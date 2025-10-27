#pragma once

#include <colors.h>
#include <font_ubuntu_mono.h>
#include <font.h>
#include <stdint.h>

typedef enum { block, hollow, line } cursor_shape;

static const bmp_font_t * user_font = &font_ubuntu_mono;
static uint32_t font_color = WHITE;
static uint32_t background_color = BLACK;

