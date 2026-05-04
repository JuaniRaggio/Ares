#pragma once

/**
 * @file process_types.h
 * @brief Shared types for process creation between kernel and userland.
 */

#include <stdint.h>

typedef struct {
        uint64_t entry;
        uint64_t argc;
        char **argv;
        char *name;
        uint64_t foreground;
        uint64_t exit_handler;
} create_process_info_t;
