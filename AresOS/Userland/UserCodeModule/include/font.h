#pragma once

#define BITMAP_WIDTH 17

typedef struct {
        const char *name;
        int width;
        int height;
        int nChars;
        const unsigned char (*bitmap)[BITMAP_WIDTH];
} bmp_font_t;

