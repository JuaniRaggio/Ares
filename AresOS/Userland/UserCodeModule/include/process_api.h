#pragma once

/**
 * @file process_api.h
 * @brief Userland process management API.
 *
 * Provides the my_* functions expected by the test suite, along with
 * a simple function registry to resolve process names to entry points.
 */

#include <stdint.h>

typedef void (*process_entry_t)();

/**
 * @brief Register a userland function so it can be found by name.
 * @param name The function name (e.g. "endless_loop").
 * @param func The function pointer.
 */
void process_register(const char *name, process_entry_t func);

/**
 * @brief Check whether a function name is registered.
 * @param name Function name to look up.
 * @return 1 if registered, 0 otherwise.
 */
int process_is_registered(const char *name);

/**
 * @brief Create a new process with full control over its attributes.
 * @param name Registered function name.
 * @param argc Argument count for the process.
 * @param argv Argument vector for the process.
 * @param foreground 1 for foreground, 0 for background.
 * @param stdin_pipe  Pipe index for stdin, or NO_PIPE for keyboard.
 * @param stdout_pipe Pipe index for stdout, or NO_PIPE for console.
 * @return PID of the new process, or -1 on failure.
 */
int64_t my_spawn(char *name, uint64_t argc, char *argv[], int foreground,
                 int stdin_pipe, int stdout_pipe);

/**
 * @brief Create a new process by function name (background, no pipes).
 * @param name Registered function name.
 * @param argc Argument count for the process.
 * @param argv Argument vector for the process.
 * @return PID of the new process, or -1 on failure.
 */
int64_t my_create_process(char *name, uint64_t argc, char *argv[]);

/**
 * @brief Human-readable reason for a failed process creation.
 * @param ret The negative value returned by my_spawn/my_create_process.
 * @return A static string describing the failure (table full, no memory, ...).
 */
const char *create_error_message(int64_t ret);

/** @brief Return the PID of the calling process. */
int64_t my_getpid(void);

/**
 * @brief Change the priority of a process.
 * @param pid Target process PID.
 * @param newPrio New priority value.
 * @return 0 on success, -1 on error.
 */
int64_t my_nice(uint64_t pid, uint64_t newPrio);

/**
 * @brief Kill a process by PID.
 * @param pid Target process PID.
 * @return 0 on success, -1 on error.
 */
int64_t my_kill(uint64_t pid);

/**
 * @brief Block a process by PID.
 * @param pid Target process PID.
 * @return 0 on success, -1 on error.
 */
int64_t my_block(uint64_t pid);

/**
 * @brief Unblock a process by PID.
 * @param pid Target process PID.
 * @return 0 on success, -1 on error.
 */
int64_t my_unblock(uint64_t pid);

/** @brief Yield the CPU to another process. */
int64_t my_yield(void);

/**
 * @brief Wait for a process to finish.
 * @param pid Target process PID.
 * @return Exit code of the process, or -1 on error.
 */
int64_t my_wait(int64_t pid);

/**
 * @brief List active process PIDs.
 * @param pids Array to fill with PIDs.
 * @param max Maximum entries to fill.
 * @return Number of PIDs written.
 */
int64_t my_list_processes(uint64_t *pids, int max);

/**
 * @brief Open or create a named pipe.
 * @param name Pipe name.
 * @return Pipe index (>= 0) on success, -1 on failure.
 */
int my_pipe_open(const char *name);

/**
 * @brief Close a named pipe.
 * @param pipe_id Pipe index.
 * @return 0 on success, -1 on error.
 */
int my_pipe_close(int pipe_id);

/**
 * @brief Create a new process with pipe redirection.
 * @param name Registered function name.
 * @param argc Argument count for the process.
 * @param argv Argument vector for the process.
 * @param stdin_pipe  Pipe index for stdin, or NO_PIPE for keyboard.
 * @param stdout_pipe Pipe index for stdout, or NO_PIPE for console.
 * @return PID of the new process, or -1 on failure.
 */
int64_t my_create_process_piped(char *name, uint64_t argc, char *argv[],
                                int stdin_pipe, int stdout_pipe);

/** @brief Idle process entry point. Yields in an infinite loop. */
void idle_process(void);
