#pragma once

#include <configuration.h>
#include <lib.h>
#include <regs.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>

#define MAX_CHARS 256
#define NULL 0
#define INVALID_COMMAND_NAME -1

typedef enum {
        CMD_HELP,
        CMD_MAN,
        CMD_INFOREG,
        CMD_TIME,
        CMD_DIV,
        CMD_CLEAR,
        CMD_PRINTMEM,
        CMD_HISTORY,
        CMD_EXIT,
        CMD_CURSOR,
        QTY_COMMANDS
} command_index;

static const char *const invalid_command = "Invalid command!\n";
static const char *const wrong_params    = "Invalid number of parameters\n";

void help(void);
void show_time(void);
void clear_cmd(void);
void print_info_reg(void);
void man(char *command);
void print_mem(char *pos);
void history_cmd(char **history);
int get_command_index(char *command);
int div_cmd(char *num, char *div);
int cursor_cmd(char *type);

typedef enum {
        supplier_t = 0,
        function_t,
        bi_function_t,
        max_parameters,
} function_type;

typedef union {
        int (*supplier)(void);
        int (*function)(char *);
        int (*bi_function)(char *, char *);
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

static const command_t help_command = {
    .name        = "help",
    .description = "List all available commands",
    .lambda =
        {
            .execute.supplier = (void *)&help,
            .ftype            = supplier_t,
        },
};

static const command_t man_command = {
    .name        = "man",
    .description = "Show manual for a specific command",
    .lambda =
        {
            .execute.function = (void *)&man,
            .ftype            = function_t,
        },
};

static const command_t inforeg_command = {
    .name        = "inforeg",
    .description = "Display captured CPU registers",
    .lambda =
        {
            .execute.supplier = (void *)&print_info_reg,
            .ftype            = supplier_t,
        },
};

static const command_t time_command = {
    .name        = "time",
    .description = "Show system elapsed time",
    .lambda =
        {
            .execute.supplier = (void *)&show_time,
            .ftype            = supplier_t,
        },
};

static const command_t div_command = {
    .name        = "div",
    .description = "Integer division of two numbers",
    .lambda =
        {
            .execute.bi_function = (void *)&div_cmd,
            .ftype               = bi_function_t,
        },
};

static const command_t clear_command = {
    .name        = "clear",
    .description = "Clear the entire screen",
    .lambda =
        {
            .execute.supplier = (void *)&clear_cmd,
            .ftype            = supplier_t,
        },
};

static const command_t print_mem_command = {
    .name        = "printmem",
    .description = "Memory dump of 32 bytes from an address",
    .lambda =
        {
            .execute.function = (void *)&print_mem,
            .ftype            = function_t,
        },
};

static const command_t history_command = {
    .name        = "history",
    .description = "Show command history",
    .lambda =
        {
            .execute.supplier = (void *)&history_cmd,
            .ftype            = function_t,
        },
};

static const command_t exit_command = {
    .name        = "exit",
    .description = "Exit Ares OS",
    .lambda =
        {
            .execute.supplier = NULL,
            .ftype            = supplier_t,
        },
};

static const command_t cursor_command = {
    .name        = "cursor",
    .description = "Change cursor shape (block, hollow, line, underline)",
    .lambda =
        {
            .execute.function = (void *)&cursor_cmd,
            .ftype            = function_t,
        },
};

static const command_t *const commands[QTY_COMMANDS] = {
    &help_command,      &man_command,     &inforeg_command,
    &time_command,      &div_command,     &clear_command,
    &print_mem_command, &history_command, &exit_command,
    &cursor_command,
};
