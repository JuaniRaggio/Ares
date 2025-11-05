#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <font.h>

/**
 * Registers a font for use
 * @param font Pointer to font structure
 */
void registerFont(bmp_font_t *font);

/**
 * Finds a font by name
 * @param name Font name
 * @return Pointer to font or NULL if not found
 */
bmp_font_t *findFont(const char *name);

/**
 * Sets the current active font
 * @param font Pointer to font structure
 */
void setFont(bmp_font_t *font);

/**
 * Sets the current active font by name
 * @param name Font name
 * @return 0 on success, -1 if font not found
 */
int setFontByName(const char *name);

/**
 * Gets the current active font
 * @return Pointer to current font
 */
bmp_font_t *getFont(void);

#endif
