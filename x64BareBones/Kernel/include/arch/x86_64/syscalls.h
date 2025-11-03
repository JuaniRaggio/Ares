// syscalls.h
#pragma once

#include <stdint.h>

#define NULL 0
#define STDIN 0
#define STDOUT 1
#define STDERR 2

// syscalls IDs
#define SYS_WRITE 0
#define SYS_EXIT 1

/* Estructura de registros en el kernel */
typedef struct {
        uint64_t rip;
        uint64_t rsp;
        uint64_t rax;
        uint64_t rbx;
        uint64_t rcx;
        uint64_t rdx;
        uint64_t rbp;
        uint64_t rdi;
        uint64_t rsi;
        uint64_t r8;
        uint64_t r9;
        uint64_t r10;
        uint64_t r11;
        uint64_t r12;
        uint64_t r13;
        uint64_t r14;
        uint64_t r15;
} regs_snapshot_t;

// syscalls
uint64_t sys_write(uint64_t fd, const char *buf, uint64_t len);
void sys_exit(uint64_t code);

/* SYS_READ - Lee un caracter del buffer de teclado */

/* SYS_CLEAR - Limpia la pantalla */
uint64_t sys_clear(void);

/* SYS_GET_TICKS - Retorna los ticks del timer */
uint64_t sys_get_ticks(void);

/* SYS_GET_SECONDS - Retorna los segundos transcurridos */
uint64_t sys_get_seconds(void);

/* SYS_GET_RESOLUTION - Retorna la resolución de pantalla */
uint64_t sys_get_resolution(uint32_t *width, uint32_t *height);

/* SYS_GET_REGISTER_ARRAY - Retorna snapshot de registros */
uint64_t sys_get_register_array(regs_snapshot_t *regs);

/* SYS_SET_FONT_SIZE - Cambia el tamaño de fuente */
uint64_t sys_set_font_size(uint8_t size);

/* SYS_GET_MEMORY - Dump de memoria */
uint64_t sys_get_memory(uint64_t addr, uint8_t *buf, uint64_t size);

/* SYS_DRAW_RECT - Dibuja un rectángulo */
uint64_t sys_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                       uint32_t color);

void init_syscalls(void);
