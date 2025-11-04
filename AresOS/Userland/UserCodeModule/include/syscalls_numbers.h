#pragma once

typedef enum {
        SYS_WRITE = 0,
        SYS_EXIT,
        SYS_READ,
        SYS_CLEAR,
        SYS_GET_TICKS,
        SYS_GET_RESOLUTION,
        SYS_GET_REGISTER_ARRAY,
        SYS_SET_FONT_SIZE,
        SYS_GET_MEMORY,
        SYS_DRAW_RECT,
        SYS_GET_SECONDS,
        SYS_SET_TEXT_COLOR,
        SYS_SET_BG_COLOR,
        SYS_GET_CURSOR_POS,
        SYS_MAX,
} syscall_codes;

