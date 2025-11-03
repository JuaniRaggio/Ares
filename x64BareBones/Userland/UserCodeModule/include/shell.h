#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>

#define RUNNING 1

extern struct regs *get_register_values();

// @breif: shows the current value of all registers
void print_registers(void);

// @brief: show input prompt in a newline
void show_input_prompt();
void welcome_shell();
int shell(void);

#endif
