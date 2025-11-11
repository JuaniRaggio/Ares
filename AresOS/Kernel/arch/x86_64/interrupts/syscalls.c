#include <drivers/sound.h>
#include <lib.h>
#include <regs.h>
#include <stdint.h>
#include <syscalls.h>

regs_snapshot_t saved_regs = {0};

uint64_t sys_read(uint64_t fd, char *buf, uint64_t count) {
        if (fd != 0 || count == 0 || buf == NULL) {
                return 0;
        }
        uint64_t i = 0;
        while (buffer_has_next() && i++ < count)
                buf[0] = buffer_next();
        return i;
}

uint64_t sys_clear(void) {
        if (videoMode == 1) {
                clearScreen(0x000000);
                screen_buffer_clear();
                gfxCursorX = 0;
                gfxCursorY = 0;
        } else {
                ncClear();
        }
        return 0;
}

uint64_t sys_get_ticks(uint64_t *ticks_ptr) {
        if (ticks_ptr == NULL) {
                return 1;
        }
        *ticks_ptr = ticks_elapsed();
        return 0;
}

uint64_t sys_get_seconds(void) {
        return seconds_elapsed();
}

uint64_t sys_get_resolution(uint32_t *width, uint32_t *height) {
        if (width == NULL || height == NULL) {
                return 1;
        }
        // TODO: implement when video data access is available
        *width  = 1024;
        *height = 768;
        return 0;
}

uint64_t sys_get_register_array(regs_snapshot_t *regs) {
        if (regs == NULL) {
                return 0;
        }
        *regs = saved_regs;
        return 0;
}

uint64_t sys_set_font_size(uint8_t scale) {
        if (scale < 1 || scale > 5) {
                return 0;
        }
        fontScale = scale;
        return 1;
}

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

uint64_t sys_draw_rect(uint64_t packed_xy, uint64_t packed_wh, uint64_t color) {
        uint16_t x      = (packed_xy >> 16) & 0xFFFF;
        uint16_t y      = packed_xy & 0xFFFF;
        uint16_t width  = (packed_wh >> 16) & 0xFFFF;
        uint16_t height = packed_wh & 0xFFFF;

        if (color == 0) {
                color = 0xFFFFFF;
        }

        drawRect(x, y, width, height, (uint32_t)color);
        return 0;
}

extern uint8_t current_stdout_color;
extern uint8_t current_stderr_color;

uint64_t sys_set_text_color(uint8_t color, uint8_t stream) {
        if (stream == 1) {
                current_stdout_color = color;
        } else if (stream == 2) {
                current_stderr_color = color;
        }
        return 0;
}

uint64_t sys_set_bg_color(uint8_t color) {
        if (videoMode == 1) {
                clearScreen(vgaToRGB(color));
                return 0;
        }
        return 1;
}

uint64_t sys_get_cursor_pos(int *x, int *y) {
        if (x == NULL || y == NULL) {
                return 0;
        }
        *x = gfxCursorX / (8 * fontScale);
        *y = gfxCursorY / (17 * fontScale);
        return 1;
}

uint64_t sys_redraw_screen(void) {
        screen_buffer_redraw();
        return 0;
}

uint64_t sys_get_time(s_time *time) {
        if (time == NULL) {
                return 1;
        }
        *time = get_current_time();
        return 0;
}

uint64_t sys_get_rdtsc(uint64_t *rdtsc) {
        *rdtsc = read_tsc();
        return 0;
}

uint64_t sys_get_time_ms(uint64_t *time_ms) {
        *time_ms = get_time_ms();
        return 0;
}

uint64_t sys_get_fps(uint64_t *fps) {
        *fps = get_current_fps();
        return 0;
}
