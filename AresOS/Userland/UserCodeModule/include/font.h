#pragma once

typedef struct {
        const char *name; // nombre legible ("ubuntu", "terminus", etc.)
        int width;
        int height;
        int nChars;
        const char *bitmap[17]; // cada carácter es un arreglo de bytes (filas)
} bmp_font_t;
