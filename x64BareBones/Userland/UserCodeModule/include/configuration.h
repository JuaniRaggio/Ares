#pragma once

#include <colors.h>
#include <font.h>
#include <font_ubuntu_mono.h>
#include <uint.h>

#define DEFAULT_PROMPT_S 32
#define DEFAULT_HISTORY_S 10
#define PROMPT_SIZE DEFAULT_PROMPT_S
#define HISTORY_SIZE DEFAULT_HISTORY_S
#define SCREEN_SIZE (TEXT_WIDTH * TEXT_HEIGHT)

typedef enum { block, hollow, line, underline } cursor_shape;

static const bmp_font_t *user_font = &font_ubuntu_mono;
static uint32_t font_color         = WHITE;
static uint32_t background_color   = BLACK;
