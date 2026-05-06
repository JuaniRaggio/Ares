#pragma once

/**
 * @file pipe.h
 * @brief Named pipe infrastructure with circular buffer.
 *
 * Provides blocking named pipes for IPC. Processes reference pipes via
 * stdin_pipe / stdout_pipe fields in the PCB.
 */

#include <stdint.h>
#include <process.h>

#define MAX_PIPES        16
#define PIPE_BUFFER_SIZE 4096
#define PIPE_NAME_LEN    32

#define PIPE_EOF     0
#define PIPE_ERR    (-1)

typedef struct {
	char name[PIPE_NAME_LEN];
	uint8_t buffer[PIPE_BUFFER_SIZE];
	int read_pos;
	int write_pos;
	int count;
	int active;
} pipe_t;

/**
 * @brief Open or create a named pipe.
 * @param name Pipe name (up to PIPE_NAME_LEN-1 chars).
 * @return Pipe index (>= 0) on success, -1 on failure.
 */
int pipe_open(const char *name);

/**
 * @brief Close a pipe. If no processes reference it, deactivate it.
 *        Wakes any processes blocked on this pipe.
 * @param pipe_id Pipe index.
 * @return 0 on success, -1 on error.
 */
int pipe_close(int pipe_id);

/**
 * @brief Read from a pipe (blocking).
 *
 * Blocks if buffer is empty and writers exist.
 * Returns 0 (EOF) if buffer is empty and no writers remain.
 *
 * @param pipe_id Pipe index.
 * @param buf Destination buffer.
 * @param count Maximum bytes to read.
 * @return Number of bytes read, or 0 on EOF.
 */
int pipe_read(int pipe_id, char *buf, int count);

/**
 * @brief Write to a pipe (blocking).
 *
 * Blocks if buffer is full and readers exist.
 * Returns -1 (broken pipe) if no readers remain.
 *
 * @param pipe_id Pipe index.
 * @param buf Source buffer.
 * @param count Number of bytes to write.
 * @return Number of bytes written, or -1 on broken pipe.
 */
int pipe_write(int pipe_id, const char *buf, int count);

/**
 * @brief Cleanup pipe references when a process dies.
 *
 * Wakes processes blocked on pipes the dying process was connected to,
 * so they can detect EOF or broken-pipe.
 *
 * @param stdin_pipe  The dying process's stdin pipe (or NO_PIPE).
 * @param stdout_pipe The dying process's stdout pipe (or NO_PIPE).
 */
void pipe_cleanup_process(int stdin_pipe, int stdout_pipe);
