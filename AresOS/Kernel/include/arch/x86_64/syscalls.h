// syscalls.h
#pragma once

#include <keyboard_driver.h>
#include <time.h>
#include <naiveConsole.h>
#include <video_driver.h>
#include <regs.h>
#include <stdint.h>
#include <syscalls_numbers.h>

#define NULL 0
#define STDIN 0
#define STDOUT 1
#define STDERR 2

// syscalls IDs
#define SYS_WRITE 0
#define SYS_EXIT 1

uint64_t sys_write(uint64_t fd, const char *buf, uint64_t len);

void sys_exit(uint64_t code);

/* SYS_READ - Lee un caracter del buffer de teclado */
uint64_t sys_read(uint64_t fd, char *buf, uint64_t count);

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
