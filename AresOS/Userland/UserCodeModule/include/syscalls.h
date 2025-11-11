#pragma once

#include <lib.h>
#include <regs.h>
#include <stdint.h>
#include <syscalls_numbers.h>

extern uint64_t _syscall3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3);

/**
 * Writes data to a file descriptor
 * @param fd File descriptor (STDOUT or STDERR)
 * @param buf Buffer containing data to write
 * @param len Number of bytes to write
 * @return Number of bytes written
 */
static inline uint64_t syscall_write(uint64_t fd, const char *buf,
                                     uint64_t len) {
        return _syscall3(SYS_WRITE, fd, (uint64_t)buf, len);
}

/**
 * Exits the program
 * @param code Exit code
 */
static inline void syscall_exit(int code) {
        _syscall3(SYS_EXIT, code, 0, 0);
}

/**
 * Reads from a file descriptor
 * @param fd File descriptor (STDIN = 0)
 * @param buf Buffer to store data
 * @param count Number of bytes to read
 * @return Number of bytes read
 */
static inline uint64_t syscall_read(uint64_t fd, char *buf, uint64_t count) {
        return _syscall3(SYS_READ, fd, (uint64_t)buf, count);
}

/**
 * Clears the screen
 * @return 0 on success
 */
static inline uint64_t syscall_clear(void) {
        return _syscall3(SYS_CLEAR, 0, 0, 0);
}

/**
 * Gets the number of timer ticks since boot
 * @param ticks_ptr Pointer to store the tick count
 * @return 0 on success, 1 on error
 */
static inline uint64_t syscall_get_ticks(uint64_t *ticks_ptr) {
        return _syscall3(SYS_GET_TICKS, (uint64_t)ticks_ptr, 0, 0);
}

/**
 * Gets the number of seconds elapsed since boot
 * @return Seconds elapsed
 */
static inline uint64_t syscall_get_seconds(void) {
        return _syscall3(SYS_GET_SECONDS, 0, 0, 0);
}

/**
 * Gets the current system time
 * @param time Pointer to time structure
 * @return 0 on success
 */
static inline uint64_t syscall_get_time(s_time *time) {
        return _syscall3(SYS_GET_TIME, (uint64_t)time, 0, 0);
}

/**
 * Gets the screen resolution
 * @param width Pointer to store screen width
 * @param height Pointer to store screen height
 * @return 0 on success
 */
static inline uint64_t syscall_get_resolution(uint32_t *width,
                                              uint32_t *height) {
        return _syscall3(SYS_GET_RESOLUTION, (uint64_t)width, (uint64_t)height,
                         0);
}

/**
 * Gets a snapshot of CPU registers
 * @param regs Pointer to register snapshot structure
 * @return 0 on success
 */
static inline uint64_t syscall_get_register_snapshot(regs_snapshot_t *regs) {
        return _syscall3(SYS_GET_REGISTER_ARRAY, (uint64_t)regs, 0, 0);
}

/**
 * Sets the font size
 * @param size Font size (8, 16, or 32)
 * @return 0 on success
 */
static inline uint64_t syscall_set_font_size(uint8_t size) {
        return _syscall3(SYS_SET_FONT_SIZE, size, 0, 0);
}

/**
 * Reads a block of memory
 * @param addr Memory address to read from
 * @param buf Buffer to store memory contents
 * @param size Number of bytes to read
 * @return 0 on success
 */
static inline uint64_t syscall_get_memory(uint64_t addr, uint8_t *buf,
                                          uint64_t size) {
        return _syscall3(SYS_GET_MEMORY, addr, (uint64_t)buf, size);
}

/**
 * Draws a rectangle on screen
 * @param x X coordinate
 * @param y Y coordinate
 * @param width Rectangle width
 * @param height Rectangle height
 * @param color Fill color
 * @return 0 on success
 */
static inline uint64_t syscall_draw_rect(uint16_t x, uint16_t y, uint16_t width,
                                         uint16_t height, uint32_t color) {
        return _syscall3(SYS_DRAW_RECT, (x << 16) | y, (width << 16) | height,
                         color);
}

/**
 * Sets the text color for a stream
 * @param color Color index
 * @param stream Output stream (1=stdout, 2=stderr)
 * @return 0 on success
 */
static inline uint64_t syscall_set_text_color(uint8_t color, uint8_t stream) {
        return _syscall3(SYS_SET_TEXT_COLOR, color, stream, 0);
}

/**
 * Sets the background color
 * @param color Color index
 * @return 0 on success
 */
static inline uint64_t syscall_set_bg_color(uint8_t color) {
        return _syscall3(SYS_SET_BG_COLOR, color, 0, 0);
}

/**
 * Gets the current cursor position
 * @param x Pointer to store X coordinate
 * @param y Pointer to store Y coordinate
 * @return 0 on success
 */
static inline uint64_t syscall_get_cursor_pos(int *x, int *y) {
        return _syscall3(SYS_GET_CURSOR_POS, (uint64_t)x, (uint64_t)y, 0);
}

/**
 * Redraws the screen with current font size
 * @return 0 on success
 */
static inline uint64_t syscall_redraw_screen(void) {
        return _syscall3(SYS_REDRAW_SCREEN, 0, 0, 0);
}

/**
 * Gets the CPU Time Stamp Counter (for high-precision benchmarking)
 * @return 64-bit TSC value (CPU cycles)
 */
static inline uint64_t syscall_get_rdtsc(void) {
        return _syscall3(SYS_GET_RDTSC, 0, 0, 0);
}

/**
 * Gets milliseconds elapsed since boot
 * @return Milliseconds elapsed
 */
static inline uint64_t syscall_get_time_ms(void) {
        return _syscall3(SYS_GET_TIME_MS, 0, 0, 0);
}

/**
 * Gets current frames per second (FPS)
 * @return Current FPS value
 */
static inline uint64_t syscall_get_fps(void) {
        return _syscall3(SYS_GET_FPS, 0, 0, 0);
}
