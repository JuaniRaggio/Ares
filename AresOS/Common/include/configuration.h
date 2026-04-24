#pragma once

#include <colors.h>
#include <stdint.h>

#define DEFAULT_HISTORY_S 10
#define HISTORY_SIZE DEFAULT_HISTORY_S

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

typedef enum {
        block,
        hollow,
        line,
        underline,
} cursor_shape;

// Default Kernel font will be used
// static const bmp_font_t *user_font = &font_ubuntu_mono;
static const uint32_t default_font_color       = WHITE; /* RGB white */
static const uint32_t default_background_color = BLACK; /* RGB black */
