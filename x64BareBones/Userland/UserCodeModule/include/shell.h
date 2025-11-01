#ifndef __SHELL_H__
#define __SHELL_H__

#include <drivers/keyboard_driver.h>
#include <drivers/video_driver.h>
#include <uint.h>

#define RUNNING 1

uint8_t get_y_cursor();
uint8_t get_x_cursor();

void shell_printf(const char *msg);
void welcome_shell();
int shell(void);

#endif
