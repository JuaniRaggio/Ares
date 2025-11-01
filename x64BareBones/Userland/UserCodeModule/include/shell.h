#ifndef __SHELL_H__
#define __SHELL_H__

#include <drivers/keyboard_driver.h>
#include <drivers/video_driver.h>
#include <stdint.h>

#define RUNNING 1

void shell_printf(const char *msg);
void welcome_shell();
int shell(void);

#endif
