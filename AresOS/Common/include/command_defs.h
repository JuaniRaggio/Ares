#pragma once

#include <stdint.h>

#define MAX_CHARS 256

typedef enum {
        supplier_t = 0,
        function_t,
        bi_function_t,
        max_parameters,
} function_type;

typedef union {
        uint8_t (*supplier)(void);
        uint8_t (*function)(char *);
        uint8_t (*bi_function)(char *, char *);
} executable_t;

typedef struct {
        executable_t execute;
        function_type ftype;
} lambda_t;

typedef struct {
        const char *name;
        const char *description;
        lambda_t lambda;
} command_t;

typedef struct {
        const command_t *cmd;
        char args[max_parameters - 1][MAX_CHARS];
} composed_command_t;
