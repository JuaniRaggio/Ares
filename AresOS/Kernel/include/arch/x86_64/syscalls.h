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
 * Reads from a file descriptor
 * @param fd File descriptor (STDIN)
 * @param buf Buffer to store the read data
 * @param count Pointer to number of bytes (input/output)
 *              Input: maximum bytes to read
 *              Output: actual bytes read
 * @return 0 on success, 1 on error
 */
uint64_t sys_read(uint64_t fd, char *buf, uint64_t *count);

/**
 * Clears the screen
 * @return 0 on success
 */
uint64_t sys_clear(void);

/**
 * Gets the number of timer ticks since boot
 * @param ticks_ptr Pointer to store the tick count
 * @return 0 on success, 1 on error
 */
uint64_t sys_get_ticks(uint64_t *ticks_ptr);

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
uint64_t sys_get_register_array(regs_snapshot_t *regs);

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
 * @param color RGB color value (0xRRGGBB)
 * @param stream Output stream (STDOUT or STDERR)
 * @return 0 on success
 */
uint64_t sys_set_text_color(uint32_t color, uint8_t stream);

/**
 * Sets the background color
 * @param color RGB color value (0xRRGGBB)
 * @return 0 on success
 */
uint64_t sys_set_bg_color(uint32_t color);

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
 * Gets the CPU Time Stamp Counter (for high-precision benchmarking)
 * @param rdtsc - output parameter to get CPU Time Stamp Counter
 * @return 64-bit TSC value
 */
uint64_t sys_get_rdtsc(uint64_t *rdtsc);

/**
 * Gets milliseconds elapsed since boot
 * @param time_ms - output parameter to get time in miliseconds since boot
 * @return status code
 */
uint64_t sys_get_time_ms(uint64_t *time_ms);

/**
 * Gets current frames per second (FPS)
 * @param fps - output parameter to get FPS
 * @return status code
 */
uint64_t sys_get_fps(uint64_t *fps);

/**
 * Initializes the syscall subsystem
 */
void init_syscalls(void);
