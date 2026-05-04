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
 * @brief Create a new process by function name.
 * @param name Registered function name.
 * @param argc Argument count for the process.
 * @param argv Argument vector for the process.
 * @return PID of the new process, or -1 on failure.
 */
int64_t my_create_process(char *name, uint64_t argc, char *argv[]);

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

/** @brief Idle process entry point. Yields in an infinite loop. */
void idle_process(void);
