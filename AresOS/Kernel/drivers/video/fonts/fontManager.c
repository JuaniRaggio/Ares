// fontManager.c

#include <fontManager.h>
#include <font_ubuntu_mono.h>
#include <lib.h>

#define MAX_FONTS 8
#define NO_FONT 0

static bmp_font_t *registeredFonts[MAX_FONTS];
static int fontCount           = 0;
static bmp_font_t *currentFont = NO_FONT;

void registerFont(bmp_font_t *font) {
        if (fontCount < MAX_FONTS)
                registeredFonts[fontCount++] = font;
}

bmp_font_t *findFont(const char *name) {
        for (int i = 0; i < fontCount; i++) {
                if (strcmp(registeredFonts[i]->name, name) == 0)
                        return registeredFonts[i];
        }
        return NO_FONT;
}

void setFont(bmp_font_t *font) {
        currentFont = font;
}

int setFontByName(const char *name) {
        bmp_font_t *font = findFont(name);
        if (font) {
                setFont(font);
                return 1;
        }
        return 0;
}

bmp_font_t *getFont() {
        return currentFont;
}
