// syscalls.h
#pragma once

#include <keyboard_driver.h>
#include <lib.h>
#include <naiveConsole.h>
#include <regs.h>
#include <stdint.h>
#include <syscalls_numbers.h>
#include <time.h>
#include <video_driver.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

/**
 * Writes data to a file descriptor
 * @param fd File descriptor (STDOUT or STDERR)
 * @param buf Buffer containing data to write
 * @param len Number of bytes to write
 * @return Number of bytes written
 */
uint64_t sys_write(uint64_t fd, const char *buf, uint64_t len);

/**
 * Exits the current process
 * @param code Exit code
 */
void sys_exit(uint64_t code);

/**
 * Reads a character from the keyboard buffer
 * @param fd File descriptor (STDIN)
 * @param buf Buffer to store the read character
 * @param count Maximum number of bytes to read
 * @return Number of bytes read
 */
uint64_t sys_read(uint64_t fd, char *buf, uint64_t count);

/**
 * Clears the screen
 * @return 0 on success
 */
uint64_t sys_clear(void);

/**
 * Gets the number of timer ticks since boot
 * @return Timer tick count
 */
uint64_t sys_get_ticks(void);

/**
 * Gets the number of seconds elapsed since boot
 * @return Seconds elapsed
 */
uint64_t sys_get_seconds(void);

/**
 * Gets the screen resolution
 * @param width Pointer to store screen width
 * @param height Pointer to store screen height
 * @return 0 on success
 */
uint64_t sys_get_resolution(uint32_t *width, uint32_t *height);

/**
 * Gets a snapshot of CPU registers
 * @param regs Pointer to structure to store register snapshot
 * @return 0 on success
 */
regs_snapshot_t sys_get_register_array(void);

/**
 * Sets the font size
 * @param size Font size (8, 16, or 32)
 * @return 0 on success
 */
uint64_t sys_set_font_size(uint8_t size);

/**
 * Reads a block of memory
 * @param addr Memory address to read from
 * @param buf Buffer to store memory contents
 * @param size Number of bytes to read
 * @return 0 on success
 */
uint64_t sys_get_memory(uint64_t addr, uint8_t *buf, uint64_t size);

/**
 * Draws a rectangle on screen
 * @param packed_xy X and Y coordinates packed in 64-bit value
 * @param packed_wh Width and height packed in 64-bit value
 * @param color Color value
 * @return 0 on success
 */
uint64_t sys_draw_rect(uint64_t packed_xy, uint64_t packed_wh, uint64_t color);

/**
 * Sets the text color for a stream
 * @param color Color index
 * @param stream Output stream (STDOUT or STDERR)
 * @return 0 on success
 */
uint64_t sys_set_text_color(uint8_t color, uint8_t stream);

/**
 * Sets the background color
 * @param color Color index
 * @return 0 on success
 */
uint64_t sys_set_bg_color(uint8_t color);

/**
 * Gets the current cursor position
 * @param x Pointer to store X coordinate
 * @param y Pointer to store Y coordinate
 * @return 0 on success
 */
uint64_t sys_get_cursor_pos(int *x, int *y);

/**
 * Redraws the screen with current font size
 * @return 0 on success
 */
uint64_t sys_redraw_screen(void);

/**
 * Gets the current system time
 * @param time Pointer to structure to store time information
 * @return 0 on success
 */
uint64_t sys_get_time(s_time *time);

/**
 * Initializes the syscall subsystem
 */
void init_syscalls(void);
