; sys_wrappers.asm
; syscall wrappers that just forward to their C handler.
; sys_exit is special (cli/hlt) and lives in sys_exit.asm.

%macro SYSCALL_WRAPPER 1
global sys_%1_wrapper
extern sys_%1
sys_%1_wrapper:
    push rbp
    mov rbp, rsp
    call sys_%1
    mov rsp, rbp
    pop rbp
    ret
%endmacro

section .text

SYSCALL_WRAPPER read
SYSCALL_WRAPPER clear
SYSCALL_WRAPPER get_ticks
SYSCALL_WRAPPER get_seconds
SYSCALL_WRAPPER get_time
SYSCALL_WRAPPER get_rdtsc
SYSCALL_WRAPPER get_time_ms
SYSCALL_WRAPPER get_fps
SYSCALL_WRAPPER get_resolution
SYSCALL_WRAPPER get_register_array
SYSCALL_WRAPPER set_font_size
SYSCALL_WRAPPER get_memory
SYSCALL_WRAPPER draw_rect
SYSCALL_WRAPPER set_text_color
SYSCALL_WRAPPER set_bg_color
SYSCALL_WRAPPER get_cursor_pos
SYSCALL_WRAPPER redraw_screen
SYSCALL_WRAPPER play_sound
SYSCALL_WRAPPER beep
SYSCALL_WRAPPER malloc
SYSCALL_WRAPPER free
SYSCALL_WRAPPER mem_stats
