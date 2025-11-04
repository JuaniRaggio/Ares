#pragma once

#include <lib.h>
#include <regs.h>
#include <stdint.h>
#include <syscalls_numbers.h>

extern uint64_t _syscall3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3);

/* Wrappers de syscalls para userspace */

/* Escritura a file descriptor */
static inline uint64_t syscall_write(uint64_t fd, const char *buf,
                                     uint64_t len) {
        return _syscall3(SYS_WRITE, fd, (uint64_t)buf, len);
}

/* Salida del programa */
static inline void syscall_exit(int code) {
        _syscall3(SYS_EXIT, code, 0, 0);
}

/* Lectura desde file descriptor (stdin = 0) */
static inline uint64_t syscall_read(uint64_t fd, char *buf, uint64_t count) {
        return _syscall3(SYS_READ, fd, (uint64_t)buf, count);
}

/* Limpia la pantalla */
static inline uint64_t syscall_clear(void) {
        return _syscall3(SYS_CLEAR, 0, 0, 0);
}

/* Obtiene los ticks del timer */
static inline uint64_t syscall_get_ticks(void) {
        return _syscall3(SYS_GET_TICKS, 0, 0, 0);
}

/* Obtiene los segundos transcurridos */
static inline uint64_t syscall_get_seconds(void) {
        return _syscall3(SYS_GET_SECONDS, 0, 0, 0);
}

/* Puts */
static inline uint64_t syscall_get_time(s_time *time) {
        return _syscall3(SYS_GET_TIME, (uint64_t)time, 0, 0);
}

/* Obtiene la resolución de pantalla */
static inline uint64_t syscall_get_resolution(uint32_t *width,
                                              uint32_t *height) {
        return _syscall3(SYS_GET_RESOLUTION, (uint64_t)width, (uint64_t)height,
                         0);
}

/* Obtiene el snapshot de registros */
static inline uint64_t syscall_get_register_snapshot(regs_snapshot_t *regs) {
        return _syscall3(SYS_GET_REGISTER_ARRAY, (uint64_t)regs, 0, 0);
}

/* Cambia el tamaño de fuente */
static inline uint64_t syscall_set_font_size(uint8_t size) {
        return _syscall3(SYS_SET_FONT_SIZE, size, 0, 0);
}

/* Dump de memoria */
static inline uint64_t syscall_get_memory(uint64_t addr, uint8_t *buf,
                                          uint64_t size) {
        return _syscall3(SYS_GET_MEMORY, addr, (uint64_t)buf, size);
}

/* Draw rectangle */
static inline uint64_t syscall_draw_rect(uint16_t x, uint16_t y, uint16_t width,
                                         uint16_t height, uint32_t color) {
        return _syscall3(SYS_DRAW_RECT, (x << 16) | y, (width << 16) | height,
                         color);
}

/* Set text color (stream: 1=stdout, 2=stderr) */
static inline uint64_t syscall_set_text_color(uint8_t color, uint8_t stream) {
        return _syscall3(SYS_SET_TEXT_COLOR, color, stream, 0);
}

/* Set background color */
static inline uint64_t syscall_set_bg_color(uint8_t color) {
        return _syscall3(SYS_SET_BG_COLOR, color, 0, 0);
}

/* Get cursor position */
static inline uint64_t syscall_get_cursor_pos(int *x, int *y) {
        return _syscall3(SYS_GET_CURSOR_POS, (uint64_t)x, (uint64_t)y, 0);
}

/* Redraw screen with current font size */
static inline uint64_t syscall_redraw_screen(void) {
        return _syscall3(SYS_REDRAW_SCREEN, 0, 0, 0);
}
