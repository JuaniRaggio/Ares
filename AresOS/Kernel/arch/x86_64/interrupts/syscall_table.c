#include <stddef.h>
#include <stdint.h>
#include <syscalls.h>
#include <syscalls_numbers.h>

/**
 * @file syscall_table.c
 * @brief Syscall dispatch table.
 *
 * Designated initializers bind each SYS_* number to its handler, so the
 * table can never drift out of sync with syscalls_numbers.h. The asm entry
 * point (syscall_entry) forwards every syscall to syscall_dispatch, which
 * bounds-checks the number before indexing the table.
 */

typedef uint64_t (*syscall_handler_t)(uint64_t, uint64_t, uint64_t);

static const syscall_handler_t syscalls_table[SYS_MAX] = {
    [SYS_WRITE]                 = (syscall_handler_t)sys_write,
    [SYS_EXIT]                  = (syscall_handler_t)sys_exit,
    [SYS_READ]                  = (syscall_handler_t)sys_read,
    [SYS_CLEAR]                 = (syscall_handler_t)sys_clear,
    [SYS_GET_TICKS]             = (syscall_handler_t)sys_get_ticks,
    [SYS_GET_RESOLUTION]        = (syscall_handler_t)sys_get_resolution,
    [SYS_GET_REGISTER_SNAPSHOT] = (syscall_handler_t)sys_get_register_array,
    [SYS_SET_FONT_SIZE]         = (syscall_handler_t)sys_set_font_size,
    [SYS_GET_MEMORY]            = (syscall_handler_t)sys_get_memory,
    [SYS_DRAW_RECT]             = (syscall_handler_t)sys_draw_rect,
    [SYS_GET_SECONDS]           = (syscall_handler_t)sys_get_seconds,
    [SYS_SET_TEXT_COLOR]        = (syscall_handler_t)sys_set_text_color,
    [SYS_SET_BG_COLOR]          = (syscall_handler_t)sys_set_bg_color,
    [SYS_GET_CURSOR_POS]        = (syscall_handler_t)sys_get_cursor_pos,
    [SYS_REDRAW_SCREEN]         = (syscall_handler_t)sys_redraw_screen,
    [SYS_GET_TIME]              = (syscall_handler_t)sys_get_time,
    [SYS_GET_RDTSC]             = (syscall_handler_t)sys_get_rdtsc,
    [SYS_GET_TIME_MS]           = (syscall_handler_t)sys_get_time_ms,
    [SYS_GET_FPS]               = (syscall_handler_t)sys_get_fps,
    [SYS_PLAY_SOUND]            = (syscall_handler_t)sys_play_sound,
    [SYS_BEEP]                  = (syscall_handler_t)sys_beep,
    [SYS_MALLOC]                = (syscall_handler_t)sys_malloc,
    [SYS_FREE]                  = (syscall_handler_t)sys_free,
    [SYS_MEM_STATS]             = (syscall_handler_t)sys_mem_stats,
    [SYS_CREATE_PROCESS]        = (syscall_handler_t)sys_create_process,
    [SYS_GETPID]                = (syscall_handler_t)sys_getpid,
    [SYS_YIELD]                 = (syscall_handler_t)sys_yield,
    [SYS_KILL]                  = (syscall_handler_t)sys_kill,
    [SYS_BLOCK]                 = (syscall_handler_t)sys_block,
    [SYS_UNBLOCK]               = (syscall_handler_t)sys_unblock,
    [SYS_NICE]                  = (syscall_handler_t)sys_nice,
    [SYS_WAITPID]               = (syscall_handler_t)sys_waitpid,
    [SYS_LIST_PROCESSES]        = (syscall_handler_t)sys_list_processes,
    [SYS_PIPE_OPEN]             = (syscall_handler_t)sys_pipe_open,
    [SYS_PIPE_CLOSE]            = (syscall_handler_t)sys_pipe_close,
    [SYS_SEM_OPEN]              = (syscall_handler_t)sys_sem_open,
    [SYS_SEM_POST]              = (syscall_handler_t)sys_sem_post,
    [SYS_SEM_WAIT]              = (syscall_handler_t)sys_sem_wait,
    [SYS_SEM_CLOSE]             = (syscall_handler_t)sys_sem_close,
    [SYS_PS]                    = (syscall_handler_t)sys_ps,
};

uint64_t syscall_dispatch(uint64_t number, uint64_t arg1, uint64_t arg2,
                          uint64_t arg3) {
        if (number >= SYS_MAX || syscalls_table[number] == NULL)
                return (uint64_t)-1;
        return syscalls_table[number](arg1, arg2, arg3);
}
