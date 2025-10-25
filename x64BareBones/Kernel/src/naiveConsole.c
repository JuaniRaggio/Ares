// naiveConsole.c

#include <stdint.h>
#include "naiveConsole.h"
#include "../video/video.h"
#include "../fonts/fontManager.h"
#include "../include/colors.h"

static uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);

// temp. buffer to print num
static char buffer[64] = {'0'};

// principal function
void ncPrint(const char *string, uint8_t color) {
    if (videoMode == 0)
        ncPrintText(string, color);
    else
        ncPrintVideo(string, color);
}

// Compatibilidad con versiones antiguas (llamadas con un solo parámetro)
void ncPrintOld(const char *string) {
    ncPrint(string, WHITE);
}

// text mode (VGA)
void ncPrintText(const char *string, uint8_t color) {
    for (int i = 0; string[i] != 0; i++)
        ncPrintCharText(string[i], color);
}

void ncPrintCharText(char c, uint8_t color) {
    *currentVideo = c;
    *(currentVideo + 1) = color;
    currentVideo += 2;
}

void ncNewline() {
    do {
        ncPrintCharText(' ', BLACK_WHITE);
    } while ((uint64_t)(currentVideo - videoTextBase) % (TEXT_WIDTH * 2) != 0);
}

void ncClear() {
    for (int i = 0; i < TEXT_WIDTH * TEXT_HEIGHT; i++)
        videoTextBase[i * 2] = ' ';
    currentVideo = videoTextBase;
}


// visual mode
void ncPrintVideo(const char *string, uint8_t color) {
    uint32_t rgb = vgaToRGB(color);
    bmp_font_t *font = getFont();

    for (int i = 0; string[i] != 0; i++) {
        drawChar(string[i], gfxCursorX, gfxCursorY, rgb, font);
        gfxCursorX += font->width;
        if (gfxCursorX + font->width >= SCREEN_WIDTH) {
            gfxCursorX = 0;
            gfxCursorY += font->height;
        }
    }
}


// num printing
void ncPrintDec(uint64_t value) { ncPrintBase(value, 10); }
void ncPrintHex(uint64_t value) { ncPrintBase(value, 16); }
void ncPrintBin(uint64_t value) { ncPrintBase(value, 2); }

void ncPrintBase(uint64_t value, uint32_t base) {
    uintToBase(value, buffer, base);
    ncPrint(buffer, WHITE);
}


// num conversion
static uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base) {
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    do {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    } while (value /= base);

    *p = 0;

    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

void ncPrintChar(char c, uint8_t color) {
    if (videoMode == 0)
        ncPrintCharText(c, color);
    else {
        uint32_t rgb = vgaToRGB(color);
        bmp_font_t *font = getFont();
        drawChar(c, gfxCursorX, gfxCursorY, rgb, font);
        gfxCursorX += font->width;
        if (gfxCursorX + font->width >= SCREEN_WIDTH) {
            gfxCursorX = 0;
            gfxCursorY += font->height;
        }
    }
}
