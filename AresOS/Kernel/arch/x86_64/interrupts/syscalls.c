/* syscalls.c - Kernel syscall implementations */
#include <syscalls.h>

/* Global register snapshot (captured by F1 hotkey) */
regs_snapshot_t saved_regs = {0};

uint64_t sys_read(uint64_t fd, char *buf, uint64_t count) {
        if (fd != 0 || count == 0 || buf == NULL) {
                return 0;
        }
        if (buffer_has_next()) {
                buf[0] = buffer_next();
                return 1;
        }
        return 0;
}

uint64_t sys_clear(void) {
        if (videoMode == 1) {
                clearScreen(0x000000);
                gfxCursorX = 0;
                gfxCursorY = 0;
        } else {
                ncClear();
        }
        return 0;
}

uint64_t sys_get_ticks(void) {
        return ticks_elapsed();
}

/* SYS_GET_SECONDS - Returns elapsed seconds */
uint64_t sys_get_seconds(void) {
        return seconds_elapsed();
}

/* SYS_GET_RESOLUTION - Returns screen resolution */
uint64_t sys_get_resolution(uint32_t *width, uint32_t *height) {
        if (width == NULL || height == NULL) {
                return 0;
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

uint64_t sys_set_font_size(uint8_t size) {
        // TODO: implement when fontManager has this functionality
        return 0;
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
        // Unpack parameters
        uint16_t x = (packed_xy >> 16) & 0xFFFF;
        uint16_t y = packed_xy & 0xFFFF;
        uint16_t width = (packed_wh >> 16) & 0xFFFF;
        uint16_t height = packed_wh & 0xFFFF;

        // If no color specified, use white as default
        if (color == 0) {
                color = 0xFFFFFF;
        }

        drawRect(x, y, width, height, (uint32_t)color);
        return 0;
}
