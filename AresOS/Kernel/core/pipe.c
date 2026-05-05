#include <lib_common.h>
#include <pipe.h>
#include <process.h>
#include <scheduler.h>

static pipe_t pipe_table[MAX_PIPES];

static int has_writers(int pipe_id) {
}

static int has_readers(int pipe_id) {
}

static void wake_blocked_on_pipe(int pipe_id) {
}

int pipe_open(const char *name) {
}

int pipe_close(int pipe_id) {
}

int pipe_read(int pipe_id, char *buf, int count) {
}

int pipe_write(int pipe_id, const char *buf, int count) {
}
void pipe_cleanup_process(int stdin_pipe, int stdout_pipe) {
}
