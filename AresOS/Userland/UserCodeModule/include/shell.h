#pragma once

#include <stdint.h>

#define RUNNING 1
#define FONT_WIDTH 8
#define FONT_HEIGHT 17
#define ZOOM_IN_CHAR 0x01
#define ZOOM_OUT_CHAR 0x02
#define MIN_MAGNIFICATION 0.5
#define MAX_MAGNIFICATION 3.0
#define MAGNIFICATION_STEP 0.25

void show_input_prompt();
void welcome_shell();
int shell(void);
int shell_read_line(char input[][256], int max_params);
