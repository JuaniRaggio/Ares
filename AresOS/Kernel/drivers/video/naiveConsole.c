// naiveConsole.c
#include <naiveConsole.h>

static uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);
// static uint8_t *const video = (uint8_t *)0xB8000;

// temp. buffer to print num
static char buffer[64] = {'0'};

void printLn(const char *str, const uint8_t color) {
        for (int i = 0; str[i] != 0; ++i) {
                ncPrintChar(str[i], color);
        }
        ncNewline();
}

void ncPrint(const char *string, uint8_t color) {
        if (videoMode == 0)
                ncPrintText(string, color);
        else
                ncPrintVideo(string, color);
}

void ncPrintOld(const char *string) {
        ncPrint(string, VGA_WHITE);
}

void ncPrintText(const char *string, uint8_t color) {
        for (int i = 0; string[i] != 0; i++)
                ncPrintCharText(string[i], color);
}

void ncPrintCharText(char c, uint8_t color) {
        *currentVideo       = c;
        *(currentVideo + 1) = color;
        currentVideo += 2;
}

void ncNewline() {
        do {
                ncPrintCharText(' ', VGA_WHITE);
        } while ((uint64_t)(currentVideo - videoTextBase) % (TEXT_WIDTH * 2) !=
                 0);
}

void ncClear() {
        for (int i = 0; i < TEXT_WIDTH * TEXT_HEIGHT; i++)
                videoTextBase[i * 2] = ' ';
        currentVideo = videoTextBase;
}

void ncPrintVideo(const char *string, uint8_t color) {
        uint32_t rgb     = vgaToRGB(color);
        bmp_font_t *font = getFont();

        for (int i = 0; string[i] != 0; i++) {
                drawChar(string[i], gfxCursorX, gfxCursorY, rgb, font);
                gfxCursorX += font->width * fontScale;
                if (gfxCursorX + font->width * fontScale >= SCREEN_WIDTH ||
                    string[i] == '\n') {
                        gfxCursorX = 0;
                        gfxCursorY += font->height * fontScale;
                        if (gfxCursorY + font->height * fontScale >= SCREEN_HEIGHT) {
                                clearScreen(0x000000);
                                gfxCursorX = 0;
                                gfxCursorY = 0;
                        }
                }
        }
}

void ncPrintDec(uint64_t value) {
        ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value) {
        ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value) {
        ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base) {
        uintToBase(value, buffer, base);
        ncPrint(buffer, VGA_WHITE);
}

static uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base) {
        char *p = buffer;
        char *p1, *p2;
        uint32_t digits = 0;
        do {
                uint32_t remainder = value % base;
                *p++ =
                    (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
                digits++;
        } while (value /= base);

        *p = 0;

        p1 = buffer;
        p2 = p - 1;

        for (char tmp = *p1; p1 < p2; ++p1, --p2, tmp = *p1) {
                *p1 = *p2;
                *p2 = tmp;
        }

        return digits;
}

void ncPrintChar(char c, uint8_t color) {
        if (videoMode == 0)
                ncPrintCharText(c, color);
        else {
                screen_buffer_add_char(c);

                uint32_t rgb     = vgaToRGB(color);
                bmp_font_t *font = getFont();
                if (font == NULL)
                        return;

                if (c == '\n') {
                        gfxCursorX = 0;
                        gfxCursorY += font->height * fontScale;
                        if (gfxCursorY + font->height * fontScale >= SCREEN_HEIGHT) {
                                clearScreen(0x000000);
                                gfxCursorX = 0;
                                gfxCursorY = 0;
                        }
                        return;
                }

                drawChar(c, gfxCursorX, gfxCursorY, rgb, font);
                gfxCursorX += font->width * fontScale;
                if (gfxCursorX + font->width * fontScale >= SCREEN_WIDTH) {
                        gfxCursorX = 0;
                        gfxCursorY += font->height * fontScale;
                        if (gfxCursorY + font->height * fontScale >= SCREEN_HEIGHT) {
                                clearScreen(0x000000);
                                gfxCursorX = 0;
                                gfxCursorY = 0;
                        }
                }
        }
}
