#include <stddef.h>
#include <lib_common.h>
#include <pipe.h>
#include <process.h>
#include <scheduler.h>
#include <status_codes.h>

static pipe_t pipe_table[MAX_PIPES];

static int has_writers(int pipe_id) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		pcb_t *p = process_get_by_index(i);
		if (p != NULL && p->stdout_pipe == pipe_id)
			return 1;
	}
	return 0;
}

static int has_readers(int pipe_id) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		pcb_t *p = process_get_by_index(i);
		if (p != NULL && p->stdin_pipe == pipe_id)
			return 1;
	}
	return 0;
}

static void wake_blocked_on_pipe(int pipe_id) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		pcb_t *p = process_get_by_index(i);
		if (p != NULL && p->state == PROCESS_BLOCKED &&
		    p->blocked_on_pipe == pipe_id) {
			p->blocked_on_pipe = NO_PIPE;
			process_unblock(p->pid);
		}
	}
}

static int find_pipe_by_name(const char *name) {
	for (int i = 0; i < MAX_PIPES; i++) {
		if (pipe_table[i].active && strcmp(pipe_table[i].name, name) == 0)
			return i;
	}
	return PIPE_ERR;
}

static int allocate_pipe(const char *name) {
	for (int i = 0; i < MAX_PIPES; i++) {
		if (!pipe_table[i].active) {
			memset(&pipe_table[i], 0, sizeof(pipe_t));
			strncpy(pipe_table[i].name, name, PIPE_NAME_LEN - 1);
			pipe_table[i].name[PIPE_NAME_LEN - 1] = '\0';
			pipe_table[i].active = 1;
			return i;
		}
	}
	return PIPE_ERR;
}

static void deactivate_if_unreferenced(int pipe_id) {
	if (!has_readers(pipe_id) && !has_writers(pipe_id)) {
		pipe_table[pipe_id].active = 0;
	}
}

static int drain_from_buffer(pipe_t *pipe, char *buf, int count) {
	int bytes_read = 0;
	while (bytes_read < count && pipe->count > 0) {
		buf[bytes_read++] = pipe->buffer[pipe->read_pos];
		pipe->read_pos = (pipe->read_pos + 1) % PIPE_BUFFER_SIZE;
		pipe->count--;
	}
	return bytes_read;
}

static int fill_into_buffer(pipe_t *pipe, const char *buf, int offset,
                            int count) {
	int bytes_written = offset;
	while (bytes_written < count && pipe->count < PIPE_BUFFER_SIZE) {
		pipe->buffer[pipe->write_pos] = buf[bytes_written++];
		pipe->write_pos = (pipe->write_pos + 1) % PIPE_BUFFER_SIZE;
		pipe->count++;
	}
	return bytes_written;
}

static void block_on_pipe(pcb_t *process, int pipe_id) {
	process->blocked_on_pipe = pipe_id;
	process_block(process->pid);
}

int pipe_open(const char *name) {
	if (name == NULL)
		return PIPE_ERR;

	int existing = find_pipe_by_name(name);
	if (existing >= 0)
		return existing;

	return allocate_pipe(name);
}

void pipe_mark_writer(int pipe_id) {
	if (pipe_id < 0 || pipe_id >= MAX_PIPES || !pipe_table[pipe_id].active)
		return;
	pipe_table[pipe_id].had_writer = 1;
}

int pipe_close(int pipe_id) {
	if (pipe_id < 0 || pipe_id >= MAX_PIPES)
		return PIPE_ERR;
	if (!pipe_table[pipe_id].active)
		return PIPE_ERR;

	wake_blocked_on_pipe(pipe_id);
	deactivate_if_unreferenced(pipe_id);

	return SYS_OK;
}

int pipe_read(int pipe_id, char *buf, int count) {
	if (pipe_id < 0 || pipe_id >= MAX_PIPES || !pipe_table[pipe_id].active)
		return PIPE_EOF;
	if (buf == NULL || count <= 0)
		return PIPE_EOF;

	pipe_t *pipe = &pipe_table[pipe_id];
	pcb_t *current = process_get_current();

	while (pipe->count == 0) {
		/* EOF only once a writer existed and they are all gone;
		 * otherwise the writer may not have been spawned yet. */
		if (pipe->had_writer && !has_writers(pipe_id))
			return PIPE_EOF;

		block_on_pipe(current, pipe_id);
		if (!pipe->active)
			return PIPE_EOF;
	}

	int bytes_read = drain_from_buffer(pipe, buf, count);
	wake_blocked_on_pipe(pipe_id);

	return bytes_read;
}

int pipe_write(int pipe_id, const char *buf, int count) {
	if (pipe_id < 0 || pipe_id >= MAX_PIPES || !pipe_table[pipe_id].active)
		return PIPE_ERR;
	if (buf == NULL || count <= 0)
		return PIPE_ERR;

	pipe_t *pipe = &pipe_table[pipe_id];
	pcb_t *current = process_get_current();

	pipe->had_writer = 1;
	int bytes_written = 0;
	while (bytes_written < count) {
		while (pipe->count == PIPE_BUFFER_SIZE) {
			if (!has_readers(pipe_id))
				return PIPE_ERR;

			block_on_pipe(current, pipe_id);
			if (!pipe->active)
				return PIPE_ERR;
		}

		if (!has_readers(pipe_id) && bytes_written == 0)
			return PIPE_ERR;

		bytes_written = fill_into_buffer(pipe, buf, bytes_written, count);
		wake_blocked_on_pipe(pipe_id);
	}

	return bytes_written;
}

void pipe_cleanup_process(int stdin_pipe, int stdout_pipe) {
	if (stdout_pipe >= 0 && stdout_pipe < MAX_PIPES &&
	    pipe_table[stdout_pipe].active) {
		wake_blocked_on_pipe(stdout_pipe);
	}

	if (stdin_pipe >= 0 && stdin_pipe < MAX_PIPES &&
	    pipe_table[stdin_pipe].active) {
		wake_blocked_on_pipe(stdin_pipe);
	}
}
