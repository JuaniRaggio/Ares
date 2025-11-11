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
extern sys_redraw_screen_wrapper
extern sys_get_time_wrapper
extern sys_get_rdtsc_wrapper
extern sys_get_time_ms_wrapper
extern sys_get_fps_wrapper
extern sys_play_sound_wrapper
extern sys_beep_wrapper

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
    dq sys_redraw_screen_wrapper ; 14: SYS_REDRAW_SCREEN
    dq sys_get_time_wrapper ; 15: SYS_GET_TIME
    dq sys_get_rdtsc_wrapper ; 16: SYS_GET_RDTSC
    dq sys_get_time_ms_wrapper ; 17: SYS_GET_TIME_MS
    dq sys_get_fps_wrapper ; 18: SYS_GET_FPS
    dq sys_play_sound_wrapper ; 19: SYS_PLAY_SOUND
    dq sys_beep_wrapper ; 20: SYS_BEEP

