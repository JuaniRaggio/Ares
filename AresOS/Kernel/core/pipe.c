#include <lib_common.h>
#include <pipe.h>
#include <process.h>
#include <scheduler.h>

static pipe_t pipe_table[MAX_PIPES];

static int has_writers(int pipe_id) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		pcb_t *p = process_get_by_index(i);
		if (p != (void *)0 && p->stdout_pipe == pipe_id)
			return 1;
	}
	return 0;
}

static int has_readers(int pipe_id) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		pcb_t *p = process_get_by_index(i);
		if (p != (void *)0 && p->stdin_pipe == pipe_id)
			return 1;
	}
	return 0;
}

static void wake_blocked_on_pipe(int pipe_id) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		pcb_t *p = process_get_by_index(i);
		if (p != (void *)0 && p->state == PROCESS_BLOCKED &&
		    p->blocked_on_pipe == pipe_id) {
			p->blocked_on_pipe = NO_PIPE;
			process_unblock(p->pid);
		}
	}
}

int pipe_open(const char *name) {
	if (name == (void *)0)
		return -1;

	/* Look for existing pipe with same name */
	for (int i = 0; i < MAX_PIPES; i++) {
		if (pipe_table[i].active && strcmp(pipe_table[i].name, name) == 0)
			return i;
	}

	/* Allocate a new pipe */
	for (int i = 0; i < MAX_PIPES; i++) {
		if (!pipe_table[i].active) {
			memset(&pipe_table[i], 0, sizeof(pipe_t));
			strncpy(pipe_table[i].name, name, PIPE_NAME_LEN - 1);
			pipe_table[i].name[PIPE_NAME_LEN - 1] = '\0';
			pipe_table[i].active = 1;
			return i;
		}
	}

	return -1;
}

int pipe_close(int pipe_id) {
	if (pipe_id < 0 || pipe_id >= MAX_PIPES)
		return -1;
	if (!pipe_table[pipe_id].active)
		return -1;

	wake_blocked_on_pipe(pipe_id);

	/* Deactivate only if no process references this pipe */
	if (!has_readers(pipe_id) && !has_writers(pipe_id)) {
		pipe_table[pipe_id].active = 0;
	}

	return 0;
}

int pipe_read(int pipe_id, char *buf, int count) {
	if (pipe_id < 0 || pipe_id >= MAX_PIPES || !pipe_table[pipe_id].active)
		return 0;
	if (buf == (void *)0 || count <= 0)
		return 0;

	pipe_t *pipe = &pipe_table[pipe_id];
	pcb_t *current = process_get_current();

	/* Block while buffer is empty and writers exist */
	while (pipe->count == 0) {
		if (!has_writers(pipe_id))
			return 0; /* EOF */

		current->blocked_on_pipe = pipe_id;
		process_block(current->pid);
		/* After unblock, re-check */
		if (!pipe->active)
			return 0;
	}

	/* Copy bytes from circular buffer */
	int bytes_read = 0;
	while (bytes_read < count && pipe->count > 0) {
		buf[bytes_read++] = pipe->buffer[pipe->read_pos];
		pipe->read_pos = (pipe->read_pos + 1) % PIPE_BUFFER_SIZE;
		pipe->count--;
	}

	/* Wake writers that were blocked because buffer was full */
	wake_blocked_on_pipe(pipe_id);

	return bytes_read;
}

int pipe_write(int pipe_id, const char *buf, int count) {
	if (pipe_id < 0 || pipe_id >= MAX_PIPES || !pipe_table[pipe_id].active)
		return -1;
	if (buf == (void *)0 || count <= 0)
		return -1;

	pipe_t *pipe = &pipe_table[pipe_id];
	pcb_t *current = process_get_current();

	int bytes_written = 0;
	while (bytes_written < count) {
		/* Block while buffer is full and readers exist */
		while (pipe->count == PIPE_BUFFER_SIZE) {
			if (!has_readers(pipe_id))
				return -1; /* Broken pipe */

			current->blocked_on_pipe = pipe_id;
			process_block(current->pid);
			if (!pipe->active)
				return -1;
		}

		if (!has_readers(pipe_id) && bytes_written == 0)
			return -1; /* Broken pipe */

		/* Copy bytes into circular buffer */
		while (bytes_written < count && pipe->count < PIPE_BUFFER_SIZE) {
			pipe->buffer[pipe->write_pos] = buf[bytes_written++];
			pipe->write_pos = (pipe->write_pos + 1) % PIPE_BUFFER_SIZE;
			pipe->count++;
		}

		/* Wake readers that were blocked because buffer was empty */
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
