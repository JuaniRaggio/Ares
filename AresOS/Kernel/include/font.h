#ifndef FONT_H
#define FONT_H

#define BITMAP_WIDTH 17

typedef struct {
        const char *name;
        int width;
        int height;
        int nChars;
        const char bitmap[][BITMAP_WIDTH];
} bmp_font_t;

#endif
