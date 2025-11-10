#pragma once

#include <configuration.h>
#include <lib.h>
#include <regs.h>
#include <status_codes.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>
#include <tron.h>

#define MAX_CHARS 256
#define NULL 0
#define INVALID_COMMAND_NAME -1

static const char *const invalid_command = "Invalid command!\n";
static const char *const wrong_params    = "Invalid number of parameters\n";

/**
 * Displays list of available commands
 * @return: status code
 */
int help(void);

/**
 * Shows current system time
 * @return: status code
 */
int show_time(void);

/**
 * Clears the screen
 * @return: status code
 */
int clear_cmd(void);

/**
 * Prints captured CPU register information
 * @return: status code
 */
int print_info_reg(void);

/**
 * Shows manual for a specific command
 * @param command Command name
 * @return: status code
 */
int man(char *command);

/**
 * Prints memory dump from specified address
 * @param pos Memory address (as string)
 * @return: status code
 */
int print_mem(char *pos);

/**
 * Displays command history
 * @return: status code
 */
int history_cmd(void);

/**
 * Gets the index of a command by name
 * @param command Command name
 * @return Command index or INVALID_COMMAND_NAME
 */
int get_command_index(char *command);

/**
 * Performs integer division
 * @param num Numerator (as string)
 * @param div Divisor (as string)
 * @return 0 on success, -1 on error
 */
int div_cmd(char *num, char *div);

/**
 * Changes cursor shape
 * @param type Cursor type (block, hollow, line, underline)
 * @return 0 on success, -1 on error
 */
int cursor_cmd(char *type);

/**
 * Launches the Tron game
 * @return: status code
 */
int tron_cmd(void);

/**
 * Triggers OPCode exception (6)
 */
int trigger_opcode_cmd(void);

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
        CMD_TRON,
        CMD_TRIGGER_OPCODE,
        QTY_COMMANDS
} command_index;

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

// Don't move to .c, shell.c depends of this
static const command_t help_command = {
    .name        = "help",
    .description = "List all available commands",
    .lambda =
        {
            .execute.supplier = &help,
            .ftype            = supplier_t,
        },
};

static const command_t man_command = {
    .name        = "man",
    .description = "Show manual for a specific command",
    .lambda =
        {
            .execute.function = &man,
            .ftype            = function_t,
        },
};

static const command_t inforeg_command = {
    .name        = "inforeg",
    .description = "Display captured CPU registers",
    .lambda =
        {
            .execute.supplier = &print_info_reg,
            .ftype            = supplier_t,
        },
};

static const command_t time_command = {
    .name        = "time",
    .description = "Show system elapsed time",
    .lambda =
        {
            .execute.supplier = &show_time,
            .ftype            = supplier_t,
        },
};

static const command_t div_command = {
    .name        = "div",
    .description = "Integer division of two numbers",
    .lambda =
        {
            .execute.bi_function = &div_cmd,
            .ftype               = bi_function_t,
        },
};

static const command_t clear_command = {
    .name        = "clear",
    .description = "Clear the entire screen",
    .lambda =
        {
            .execute.supplier = &clear_cmd,
            .ftype            = supplier_t,
        },
};

static const command_t print_mem_command = {
    .name        = "printmem",
    .description = "Memory dump of 32 bytes from an address",
    .lambda =
        {
            .execute.function = &print_mem,
            .ftype            = function_t,
        },
};

static const command_t history_command = {
    .name        = "history",
    .description = "Show command history",
    .lambda =
        {
            .execute.supplier = &history_cmd,
            .ftype            = supplier_t,
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
            .execute.function = &cursor_cmd,
            .ftype            = function_t,
        },
};

static const command_t tron_command = {
    .name        = "tron",
    .description = "Play the Tron game (WASD vs IJKL)",
    .lambda =
        {
            .execute.supplier = &tron_cmd,
            .ftype            = supplier_t,
        },
};

static const command_t trigger_opcode = {
    .name        = "opcode",
    .description = "Triggers an invalid opcode exception",
    .lambda =
        {
            .ftype            = supplier_t,
            .execute.supplier = &trigger_opcode_cmd,
        },
};

static const command_t *const commands[QTY_COMMANDS] = {
    &help_command, &man_command,    &inforeg_command,   &time_command,
    &div_command,  &clear_command,  &print_mem_command, &history_command,
    &exit_command, &cursor_command, &tron_command,      &trigger_opcode,
};
