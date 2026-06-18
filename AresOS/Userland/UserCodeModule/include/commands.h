#pragma once

#include <stddef.h>
#include <command_defs.h>
#include <configuration.h>
#include <lib.h>
#include <regs.h>
#include <status_codes.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>
#include <tron.h>

static const char *const invalid_command = "Invalid command!\n";

/**
 * Displays list of available commands
 * @return: status code
 */
uint8_t help(void);

/**
 * Shows current system time
 * @return: status code
 */
uint8_t show_time(void);

/**
 * Clears the screen
 * @return: status code
 */
uint8_t clear_cmd(void);

/**
 * Prints captured CPU register information
 * @return: status code
 */
uint8_t print_info_reg(void);

/**
 * Shows manual for a specific command
 * @param command Command name
 * @return: status code
 */
uint8_t man(char *command);

/**
 * Displays command history
 * @return: status code
 */
uint8_t history_cmd(void);

/**
 * Changes cursor shape
 * @param type Cursor type (block, hollow, line, underline)
 * @return 0 on success, -1 on error
 */
uint8_t cursor_cmd(char *type);

/**
 * Changes text color
 * @param color Color name or hex value
 * @return: status code
 */
uint8_t textcolor_cmd(char *color);

/**
 * Changes background color
 * @param color Color name or hex value
 * @return: status code
 */
uint8_t bgcolor_cmd(char *color);

/**
 * Informs that the shell cannot exit
 * @return: status code
 */
uint8_t exit_cmd(void);
