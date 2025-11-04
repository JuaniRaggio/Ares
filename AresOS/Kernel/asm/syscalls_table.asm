section .data
global syscalls_table
extern sys_write
extern sys_exit
extern sys_read_wrapper
extern sys_clear_wrapper
extern sys_get_ticks_wrapper
extern sys_get_resolution_wrapper
extern sys_get_register_array_wrapper
extern sys_set_font_size_wrapper
extern sys_get_memory_wrapper
extern sys_draw_rect_wrapper
extern sys_get_seconds_wrapper
extern sys_set_text_color_wrapper
extern sys_set_bg_color_wrapper
extern sys_get_cursor_pos_wrapper

syscalls_table:
    dq sys_write              ; 0: SYS_WRITE
    dq sys_exit               ; 1: SYS_EXIT
    dq sys_read_wrapper       ; 2: SYS_READ
    dq sys_clear_wrapper      ; 3: SYS_CLEAR
    dq sys_get_ticks_wrapper  ; 4: SYS_GET_TICKS
    dq sys_get_resolution_wrapper ; 5: SYS_GET_RESOLUTION
    dq sys_get_register_array_wrapper ; 6: SYS_GET_REGISTER_ARRAY
    dq sys_set_font_size_wrapper ; 7: SYS_SET_FONT_SIZE
    dq sys_get_memory_wrapper ; 8: SYS_GET_MEMORY
    dq sys_draw_rect_wrapper  ; 9: SYS_DRAW_RECT
    dq sys_get_seconds_wrapper ; 10: SYS_GET_SECONDS
    dq sys_set_text_color_wrapper ; 11: SYS_SET_TEXT_COLOR
    dq sys_set_bg_color_wrapper ; 12: SYS_SET_BG_COLOR
    dq sys_get_cursor_pos_wrapper ; 13: SYS_GET_CURSOR_POS

