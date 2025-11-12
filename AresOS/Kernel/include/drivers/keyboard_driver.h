#pragma once

#include <naiveConsole.h>
#include <stdint.h>

/**
 * Keyboard driver for PS/2 keyboard
 * Handles keyboard input, scan code translation, and input buffering
 */

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

/**
 * Handles keyboard interrupt
 * @param stack_ptr Pointer to stack for register capture (Ctrl+R)
 * @return Status code
 */
uint8_t keyboard_handler(uint64_t *stack_ptr);

/**
 * Adds a character to the keyboard input buffer
 * @param c Character to add
 */
void update_buffer(uint8_t c);

/**
 * Checks if the keyboard buffer has data available
 * @return 1 if data available, 0 otherwise
 */
uint8_t buffer_has_next();

/**
 * Gets the next character from the keyboard buffer
 * @return Next character from buffer
 */
uint8_t buffer_next();

/**
 * Captures current CPU register state (triggered by Ctrl+R)
 * @param stack_ptr Pointer to stack containing register values
 */
void capture_registers(uint64_t *stack_ptr);

/**
 * Reads input from keyboard port
 * @return Scan code from keyboard
 */
extern uint8_t get_input();
