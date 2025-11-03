#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>

#define RUNNING 1

// @brief: show input prompt in a newline
void show_input_prompt();
void welcome_shell();
int shell(void);

#endif
