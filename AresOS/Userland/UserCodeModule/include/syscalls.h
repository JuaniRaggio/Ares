#pragma once

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

/* Obtiene la resolución de pantalla */
static inline uint64_t syscall_get_resolution(uint32_t *width,
                                              uint32_t *height) {
        return _syscall3(SYS_GET_RESOLUTION, (uint64_t)width, (uint64_t)height,
                         0);
}

/* Forward declaration de la struct de registros */
struct regs_snapshot;

/* Obtiene el snapshot de registros */
static inline uint64_t
syscall_get_register_snapshot(struct regs_snapshot *regs) {
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

/* Dibuja un rectángulo */
static inline uint64_t syscall_draw_rect(uint16_t x, uint16_t y, uint16_t width,
                                         uint16_t height) {
        return _syscall3(SYS_DRAW_RECT, (x << 16) | y, (width << 16) | height,
                         0);
}
