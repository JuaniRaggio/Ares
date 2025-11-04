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

void show_input_prompt();
void welcome_shell();
int shell(void);
int shell_read_line(char input[][256], int max_params);
s_time get_shell_start_time(void);
