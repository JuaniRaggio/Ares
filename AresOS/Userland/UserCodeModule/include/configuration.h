#pragma once

#include <stdint.h>

#define DEFAULT_HISTORY_S 10
#define HISTORY_SIZE DEFAULT_HISTORY_S

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

#define BLACK 0x0
#define BLUE 0x1
#define GREEN 0x2
#define CYAN 0x3
#define RED 0x4
#define MAGENTA 0x5
#define BROWN 0x6
#define GREY 0x7
#define WHITE 0xF
#define BLACK_WHITE 0x0F
#define GREEN_BLACK 0x20
#define RED_WHITE 0x00FF0000

typedef enum {
        block,
        hollow,
        line,
        underline,
} cursor_shape;

// Default Kernel font will be used
// static const bmp_font_t *user_font = &font_ubuntu_mono;
static const uint32_t default_font_color       = WHITE;
static const uint32_t default_background_color = BLACK;
