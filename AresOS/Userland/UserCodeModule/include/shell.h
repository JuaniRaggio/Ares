#pragma once

#include <stdint.h>

#define RUNNING 1
#define FONT_WIDTH 8
#define FONT_HEIGHT 17

void show_input_prompt();
void welcome_shell();
int shell(void);
int shell_read_line(char input[][256], int max_params);
