#pragma once

/**
 * @file process_types.h
 * @brief Shared types for process creation between kernel and userland.
 */

#include <stdint.h>

#define NO_PIPE (-1)

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
