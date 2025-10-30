// fontManager.h

#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include "font.h"

void registerFont(bmp_font_t *font);
bmp_font_t *findFont(const char *name);
void setFont(bmp_font_t *font);
int setFontByName(const char *name);
bmp_font_t *getFont(void);

#endif
