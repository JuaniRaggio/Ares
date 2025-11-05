#pragma once

#include <lib.h>
#include <stdint.h>

#define RUNNING 1
#define FONT_WIDTH 8
#define FONT_HEIGHT 17
#define ZOOM_IN_CHAR 0x01
#define ZOOM_OUT_CHAR 0x02
#define MIN_FONT_SCALE 1
#define MAX_FONT_SCALE 5

/**
 * Displays the shell input prompt
 */
void show_input_prompt();

/**
 * Displays the welcome message
 */
void welcome_shell();

/**
 * Main shell loop
 * @return Exit status
 */
int shell(void);

/**
 * Reads a command line and splits it into parameters
 * @param input Array to store command and parameters
 * @param max_params Maximum number of parameters
 * @return Number of parameters read
 */
int shell_read_line(char input[][256], int max_params);

/**
 * Gets the time when the shell was started
 * @return Shell start time
 */
s_time get_shell_start_time(void);
