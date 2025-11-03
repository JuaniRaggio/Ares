#pragma once

#include <stdint.h>

#define DEFAULT_PROMPT_S 32
#define DEFAULT_HISTORY_S 10
#define PROMPT_SIZE DEFAULT_PROMPT_S
#define HISTORY_SIZE DEFAULT_HISTORY_S

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
// #define CELL_SIZE 10
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

// static const bmp_font_t *user_font = &font_ubuntu_mono;
static uint32_t font_color       = WHITE;
static uint32_t background_color = BLACK;
