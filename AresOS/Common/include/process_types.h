#pragma once

/**
 * @file process_types.h
 * @brief Shared types for process creation between kernel and userland.
 */

#include <stdint.h>

/* pid sentinel: returned where a function yields a pid but none exists
 * (failed spawn, empty queue). Shared so kernel and userland agree on it. */
#define NO_PID  (-1)
#define NO_PIPE (-1)

#define PROCESS_INFO_NAME_LEN 32

/** @brief Process states. Shared so userland can decode ps snapshots. */
typedef enum {
        PROCESS_READY   = 0,
        PROCESS_RUNNING = 1,
        PROCESS_BLOCKED = 2,
        PROCESS_DEAD    = 3,
        PROCESS_ZOMBIE  = 4
} process_state_t;

typedef struct {
        uint64_t entry;
        uint64_t argc;
        char **argv;
        char *name;
        uint64_t foreground;
        uint64_t exit_handler;
        int stdin_pipe;
        int stdout_pipe;
} create_process_info_t;

/** @brief Per-process snapshot returned by the SYS_PS syscall. */
typedef struct {
        int64_t pid;
        char name[PROCESS_INFO_NAME_LEN];
        uint64_t priority;
        uint64_t rsp;        /* saved stack pointer */
        uint64_t stack_base; /* base of the user stack */
        int state;           /* process_state_t value */
        int foreground;
} process_info_t;
