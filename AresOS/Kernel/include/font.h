#ifndef FONT_H
#define FONT_H

typedef struct {
        const char *name; // nombre legible ("ubuntu", "terminus", etc.)
        int width;
        int height;
        int nChars;
        const unsigned char
            *bitmap[17]; // cada carácter es un arreglo de bytes (filas)
} bmp_font_t;

#endif
