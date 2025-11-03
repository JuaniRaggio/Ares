#pragma once

#include <configuration.h>
#include <lib.h>
#include <regs.h>
#include <stdint.h>
#include <stdio.h>
#include <syscall.h>

#define NULL 0
#define QTY_COMMANDS 9

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

typedef enum {
        supplier_t = 0,
        function_t,
        bi_function_t,
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
        char *name;
        char *description;
        lambda_t lambda;
} command_t;

static command_t commands[QTY_COMMANDS] = {
    {
        .name        = "help",
        .description = "List all available commands",
        .lambda =
            {
                .execute.supplier = (void *)&help,
                .ftype            = supplier_t,
            },
    },
    {
        .name        = "man",
        .description = "Show manual for a specific command",
        .lambda =
            {
                .execute.function = (void *)&man,
                .ftype            = function_t,
            },
    },
    {
        .name        = "inforeg",
        .description = "Display captured CPU registers",
        .lambda =
            {
                .execute.supplier = (void *)&print_info_reg,
                .ftype            = supplier_t,
            },
    },
    {
        .name        = "time",
        .description = "Show system elapsed time",
        .lambda =
            {
                .execute.supplier = (void *)&show_time,
                .ftype            = supplier_t,
            },
    },
    {
        .name        = "div",
        .description = "Integer division of two numbers",
        .lambda =
            {
                .execute.bi_function = (void *)&div_cmd,
                .ftype               = bi_function_t,
            },
    },
    {
        .name        = "clear",
        .description = "Clear the entire screen",
        .lambda =
            {
                .execute.supplier = (void *)&clear_cmd,
                .ftype            = supplier_t,
            },
    },
    {
        .name        = "printmem",
        .description = "Memory dump of 32 bytes from an address",
        .lambda =
            {
                .execute.function = (void *)&print_mem,
                .ftype            = function_t,
            },
    },
    {
        .name        = "history",
        .description = "Show command history",
        .lambda =
            {
                .execute.supplier = (void *)&history_cmd,
                .ftype            = function_t,
            },
    },
    {
        .name        = "exit",
        .description = "Exit Ares OS",
        .lambda =
            {
                .execute.supplier = NULL,
                .ftype            = supplier_t,
            },
    },
};
