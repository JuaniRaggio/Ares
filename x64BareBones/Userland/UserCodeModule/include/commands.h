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
        supplier = 0,
        function,
        bi_function,
} function_type;

typedef struct {
        char *name;
        char *description;
        union {
                int (*f)(void);
                int (*g)(char *);
                int (*h)(char *, char *);
        };
        function_type ftype;
} command_t;

static command_t commands[QTY_COMMANDS] = {
    {
        .name        = "help",
        .description = "List all available commands",
        .f           = (void *)&help,
        .ftype       = supplier,
    },
    {
        .name        = "man",
        .description = "Show manual for a specific command",
        .g           = (void *)&man,
        .ftype       = function,
    },
    {
        .name        = "inforeg",
        .description = "Display captured CPU registers",
        .f           = (void *)&print_info_reg,
        .ftype       = supplier,
    },
    {
        .name        = "time",
        .description = "Show system elapsed time",
        .f           = (void *)&show_time,
        .ftype       = supplier,
    },
    {
        .name        = "div",
        .description = "Integer division of two numbers",
        .h           = (void *)&div_cmd,
        .ftype       = bi_function,
    },
    {
        .name        = "clear",
        .description = "Clear the entire screen",
        .f           = (void *)&clear_cmd,
        .ftype       = supplier,
    },
    {
        .name        = "printmem",
        .description = "Memory dump of 32 bytes from an address",
        .g           = (void *)&print_mem,
        .ftype       = function,
    },
    {
        .name        = "history",
        .description = "Show command history",
        .f           = (void *)&history_cmd,
        .ftype       = function,
    },
    {
        .name        = "exit",
        .description = "Exit Ares OS",
        .f           = NULL,
        .ftype       = supplier,
    },
};
