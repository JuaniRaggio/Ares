/* syscalls.c - Implementaciones de syscalls del kernel */
#include <drivers/keyboard_driver.h>
#include <drivers/time.h>
#include <naiveConsole.h>
#include <stdint.h>
#include <syscall_numbers.h>

/* SYS_READ - Lee un caracter del buffer de teclado */
uint64_t sys_read(uint64_t fd, char *buf, uint64_t count) {
        if (fd != 0 || count == 0 || buf == NULL) {
                return 0;
        }

        // Leer del buffer de teclado
        if (buffer_has_next()) {
                buf[0] = buffer_next();
                return 1;
        }

        return 0;
}

/* SYS_CLEAR - Limpia la pantalla */
uint64_t sys_clear(void) {
        ncClear();
        return 0;
}

/* SYS_GET_TICKS - Retorna los ticks del timer */
uint64_t sys_get_ticks(void) {
        return ticks_elapsed();
}

/* SYS_GET_SECONDS - Retorna los segundos transcurridos */
uint64_t sys_get_seconds(void) {
        return seconds_elapsed();
}

/* SYS_GET_RESOLUTION - Retorna la resolución de pantalla */
uint64_t sys_get_resolution(uint32_t *width, uint32_t *height) {
        if (width == NULL || height == NULL) {
                return 0;
        }
        // TODO: implementar cuando se tenga acceso a los datos de video
        *width  = 1024;
        *height = 768;
        return 0;
}

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

/* Snapshot global de registros (capturado por hotkey) */
static regs_snapshot_t saved_regs = {0};

/* SYS_GET_REGISTER_ARRAY - Retorna snapshot de registros */
uint64_t sys_get_register_array(regs_snapshot_t *regs) {
        if (regs == NULL) {
                return 0;
        }
        /* Copiar el snapshot guardado al buffer del usuario */
        *regs = saved_regs;
        return 0;
}

/* SYS_SET_FONT_SIZE - Cambia el tamaño de fuente */
uint64_t sys_set_font_size(uint8_t size) {
        // TODO: implementar cuando fontManager tenga esta funcionalidad
        return 0;
}

/* SYS_GET_MEMORY - Dump de memoria */
uint64_t sys_get_memory(uint64_t addr, uint8_t *buf, uint64_t size) {
        if (buf == NULL || size == 0) {
                return 0;
        }

        uint8_t *ptr = (uint8_t *)addr;
        for (uint64_t i = 0; i < size; i++) {
                buf[i] = ptr[i];
        }

        return size;
}

/* SYS_DRAW_RECT - Dibuja un rectángulo */
uint64_t sys_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                       uint32_t color) {
        // TODO: implementar cuando video_driver tenga esta funcionalidad
        return 0;
}
