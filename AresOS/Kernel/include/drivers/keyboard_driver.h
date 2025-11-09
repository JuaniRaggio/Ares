#pragma once

#include <naiveConsole.h>
#include <stdint.h>

#define TABLE_SIZE 256
#define NON_PRINTABLE 0
#define ZOOM_IN_CHAR 0x01
#define ZOOM_OUT_CHAR 0x02

#define LSHIFT_CODE 0x2A
#define RSHIFT_CODE 0x36
#define LCTRL_CODE 0x1D
#define R_CODE 0x13
#define MINUS_CODE 0x0C
#define EQUALS_CODE 0x0D
#define BREAK_CODE 0x80

uint8_t keyboard_handler(uint64_t *stack_ptr);
void update_buffer(uint8_t c);

uint8_t buffer_has_next();
uint8_t buffer_next();

void capture_registers(uint64_t *stack_ptr);

extern uint8_t get_input();
