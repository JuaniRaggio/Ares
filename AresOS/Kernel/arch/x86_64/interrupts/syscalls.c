#include <stddef.h>
#include <drivers/keyboard_driver.h>
#include <drivers/sound.h>
#include <interrupts.h>
#include <lib.h>
#include <memory_manager.h>
#include <naiveConsole.h>
#include <pipe.h>
#include <process.h>
#include <process_types.h>
#include <regs.h>
#include <scheduler.h>
#include <semaphores.h>
#include <status_codes.h>
#include <stdint.h>
#include <syscalls.h>

#define DEFAULT_BG_COLOR 0x000000
#define DEFAULT_STDOUT_COLOR 0xFFFFFF
#define DEFAULT_STDERR_COLOR 0xFF0000

regs_snapshot_t saved_regs               = {0};
uint32_t current_bg_color                = DEFAULT_BG_COLOR;
static uint32_t current_stdout_color_rgb = DEFAULT_STDOUT_COLOR;
static uint32_t current_stderr_color_rgb = DEFAULT_STDERR_COLOR;

static int try_write_to_pipe(const char *buf, uint64_t len) {
        pcb_t *current = process_get_current();
        if (current == NULL || current->stdout_pipe < 0)
                return 0;
        int ret = pipe_write(current->stdout_pipe, buf, (int)len);
        return (ret < 0) ? 0 : ret;
}

static int stdout_is_piped(void) {
        pcb_t *current = process_get_current();
        return current != NULL && current->stdout_pipe >= 0;
}

uint64_t sys_write(uint64_t fd, const char *buf, uint64_t len) {
        if (fd != 1 && fd != 2) {
                return SYS_OK;
        }
        if (buf == NULL || len == 0) {
                return SYS_OK;
        }

        if (fd == 1 && stdout_is_piped()) {
                return (uint64_t)try_write_to_pipe(buf, len);
        }

        uint32_t color =
            (fd == 1) ? current_stdout_color_rgb : current_stderr_color_rgb;

        for (uint64_t i = 0; i < len; i++) {
                ncPrintCharRGB(buf[i], color);
        }

        return len;
}

static uint64_t drain_keyboard_buffer(char *buf, uint64_t max_count) {
        uint64_t i = 0;
        while (buffer_has_next() && i < max_count) {
                buf[i] = buffer_next();
                i++;
        }
        return i;
}

/* Blocks until the keyboard IRQ delivers input or an EOF (Ctrl+D).
 * Interrupts are disabled around the check to avoid losing a wakeup
 * between testing the buffer and blocking.
 * Returns 1 when there is data to read, 0 on end of file. */
static int wait_for_keyboard_input(pcb_t *current) {
        while (1) {
                _cli();
                if (buffer_has_next()) {
                        _sti();
                        return 1;
                }
                if (buffer_consume_eof()) {
                        _sti();
                        return 0;
                }
                if (current != NULL) {
                        current->blocked_on_keyboard = 1;
                        process_block(current->pid);
                }
                _sti();
                _hlt();
        }
}

uint64_t sys_read(uint64_t fd, char *buf, uint64_t *count) {
        if ((fd != STDIN && fd != FD_KBD_NONBLOCK) || count == NULL ||
            buf == NULL) {
                if (count != NULL) {
                        *count = 0;
                }
                return SYS_BAD;
        }

        uint64_t max_count = *count;
        if (max_count == 0) {
                *count = 0;
                return SYS_BAD;
        }

        pcb_t *current = process_get_current();
        if (fd == STDIN && current != NULL && current->stdin_pipe >= 0) {
                int ret = pipe_read(current->stdin_pipe, buf, (int)max_count);
                *count  = (ret > 0) ? (uint64_t)ret : 0;
                return (ret >= 0) ? SYS_OK : SYS_BAD;
        }

        if (fd == FD_KBD_NONBLOCK) {
                *count = drain_keyboard_buffer(buf, max_count);
                return SYS_OK;
        }

        /* Background processes get EOF instead of stealing keystrokes. */
        if (current != NULL && !current->foreground) {
                *count = 0;
                return SYS_OK;
        }

        if (!wait_for_keyboard_input(current)) {
                *count = 0; /* Ctrl+D: end of file */
                return SYS_OK;
        }

        *count = drain_keyboard_buffer(buf, max_count);
        return SYS_OK;
}

uint64_t sys_clear(void) {
        if (videoMode == 1) {
                clearScreen(current_bg_color);
                screen_buffer_clear();
                gfxCursorX = 0;
                gfxCursorY = 0;
        } else {
                ncClear();
        }
        return SYS_OK;
}

uint64_t sys_get_ticks(uint64_t *ticks_ptr) {
        if (ticks_ptr == NULL) {
                return SYS_BAD;
        }
        *ticks_ptr = ticks_elapsed();
        return SYS_OK;
}

uint64_t sys_get_seconds(void) {
        return seconds_elapsed();
}

uint64_t sys_get_resolution(uint32_t *width, uint32_t *height) {
        if (width == NULL || height == NULL) {
                return SYS_BAD;
        }
        // TODO: implement when video data access is available
        *width  = 1024;
        *height = 768;
        return SYS_OK;
}

uint64_t sys_get_register_array(regs_snapshot_t *regs) {
        if (regs == NULL) {
                return SYS_BAD;
        }
        *regs = saved_regs;
        return SYS_OK;
}

uint64_t sys_set_font_size(uint8_t scale) {
        if (scale < 1 || scale > 5) {
                return SYS_BAD;
        }
        fontScale = scale;
        return SYS_OK;
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

        /* No remapping of color 0: black is a valid color (used to erase the
         * cursor against a black background). */
        drawRect(x, y, width, height, (uint32_t)color);
        return SYS_OK;
}

uint64_t sys_set_text_color(uint32_t color, uint8_t stream) {
        if (stream == 1) {
                current_stdout_color_rgb = color;
        } else if (stream == 2) {
                current_stderr_color_rgb = color;
        }
        return SYS_OK;
}

uint64_t sys_set_bg_color(uint32_t color) {
        if (videoMode == 1) {
                current_bg_color = color;
                clearScreen(current_bg_color);
                return SYS_OK;
        }
        return SYS_BAD;
}

uint64_t sys_get_cursor_pos(int *x, int *y) {
        if (x == NULL || y == NULL) {
                return SYS_BAD;
        }
        *x = gfxCursorX / (8 * fontScale);
        *y = gfxCursorY / (17 * fontScale);
        return SYS_OK;
}

uint64_t sys_redraw_screen(void) {
        screen_buffer_redraw();
        return SYS_OK;
}

uint64_t sys_get_time(s_time *time) {
        if (time == NULL) {
                return SYS_BAD;
        }
        *time = get_current_time();
        return SYS_OK;
}

uint64_t sys_get_rdtsc(uint64_t *rdtsc) {
        *rdtsc = read_tsc();
        return SYS_OK;
}

uint64_t sys_get_time_ms(uint64_t *time_ms) {
        *time_ms = get_time_ms();
        return SYS_OK;
}

uint64_t sys_get_fps(uint64_t *fps) {
        *fps = get_current_fps();
        return SYS_OK;
}

uint64_t sys_play_sound(uint64_t frequency, uint64_t duration_ms) {
        playSound(frequency, duration_ms);
        return SYS_OK;
}

uint64_t sys_beep(uint64_t frequency) {
        beep(frequency);
        return SYS_OK;
}

/* User allocations carry a 16-byte header (a list node) so the kernel can free
 * them if the process dies without freeing. The header is 16 bytes, so the
 * returned pointer stays 16-byte aligned. */
uint64_t sys_malloc(uint64_t size) {
        user_alloc_node_t *node =
            (user_alloc_node_t *)mem_alloc(sizeof(user_alloc_node_t) + size);
        if (node == NULL)
                return 0;

        pcb_t *self             = process_get_current();
        user_alloc_node_t *head = &self->user_allocs;
        node->next              = head->next;
        node->prev              = head;
        head->next->prev        = node;
        head->next              = node;

        return (uint64_t)(node + 1);
}

uint64_t sys_free(uint64_t ptr) {
        if (ptr == 0)
                return SYS_OK;
        user_alloc_node_t *node = (user_alloc_node_t *)ptr - 1;
        node->prev->next        = node->next;
        node->next->prev        = node->prev;
        mem_free(node);
        return SYS_OK;
}

uint64_t sys_mem_stats(uint64_t stats_ptr) {
        if (stats_ptr == 0)
                return SYS_BAD;
        mem_get_stats((heap_stats_t *)stats_ptr);
        return SYS_OK;
}

void sys_exit(uint64_t code) {
        process_exit((int)code);
}

uint64_t sys_create_process(uint64_t info_ptr) {
        if (info_ptr == 0)
                return (uint64_t)NO_PID;
        create_process_info_t *info = (create_process_info_t *)info_ptr;
        return (uint64_t)process_create(
            info->entry, info->argc, info->argv, info->name, info->foreground,
            info->exit_handler, info->stdin_pipe, info->stdout_pipe);
}

uint64_t sys_getpid(void) {
        return (uint64_t)process_getpid();
}

uint64_t sys_yield(void) {
        /* Immediate reschedule (no _hlt): the caller gives up the CPU now and
         * is resumed as soon as the scheduler picks it again. Without the fixed
         * one-tick floor that _hlt imposed, how fast a yielding process comes
         * back is governed purely by its scheduling frequency (priority). */
        _yield_now();
        return SYS_OK;
}

uint64_t sys_kill(uint64_t pid) {
        return (uint64_t)process_kill((pid_t)pid);
}

uint64_t sys_block(uint64_t pid) {
        return (uint64_t)process_block((pid_t)pid);
}

uint64_t sys_unblock(uint64_t pid) {
        return (uint64_t)process_unblock((pid_t)pid);
}

uint64_t sys_nice(uint64_t pid, uint64_t new_priority) {
        return (uint64_t)process_nice((pid_t)pid, new_priority);
}

uint64_t sys_waitpid(uint64_t pid) {
        return (uint64_t)process_wait((pid_t)pid);
}

uint64_t sys_list_processes(uint64_t pids_ptr, uint64_t max_count) {
        if (pids_ptr == 0)
                return 0;
        return (uint64_t)process_list((uint64_t *)pids_ptr, (int)max_count);
}

uint64_t sys_ps(uint64_t info_ptr, uint64_t max_count) {
        if (info_ptr == 0)
                return 0;
        return (uint64_t)process_snapshot((process_info_t *)info_ptr,
                                          (int)max_count);
}

uint64_t sys_pipe_open(uint64_t name_ptr) {
        if (name_ptr == 0)
                return (uint64_t)PIPE_ERR;
        return (uint64_t)pipe_open((const char *)name_ptr);
}

uint64_t sys_pipe_close(uint64_t pipe_id) {
        return (uint64_t)pipe_close((int)pipe_id);
}

uint64_t sys_sem_open(char *sem_id, uint64_t value) {
        return (uint64_t)sem_open(sem_id, value);
}

uint64_t sys_sem_post(char *sem_idx) {
        return (uint64_t)sem_post(sem_idx);
}

uint64_t sys_sem_wait(char *sem_idx) {
        return (uint64_t)sem_wait(sem_idx);
}

uint64_t sys_sem_close(char *sem_idx) {
        return (uint64_t)sem_close(sem_idx);
}
