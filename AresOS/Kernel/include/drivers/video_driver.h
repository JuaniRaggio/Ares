#pragma once

#include <colors.h>
#include <font.h>
#include <fontManager.h>
#include <font_ubuntu_mono.h>
#include <naiveConsole.h>
#include <stdint.h>

#define NULL ((void *)0)

#define VIDEO_ADDR_GFX 0x000A0000
#define VIDEO_ADDR_TEXT 0xB8000

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define TEXT_WIDTH 80
#define TEXT_HEIGHT 25

extern uint8_t videoMode;
extern uint32_t *framebuffer;
extern uint8_t *videoTextBase;
extern uint8_t *currentVideo;
extern int gfxCursorX, gfxCursorY;

extern uint32_t screenWidth;
extern uint32_t screenHeight;
extern uint8_t bytesPerPixel;
extern uint8_t fontScale;

/**
 * Initializes the video subsystem
 */
void video_init(void);

/**
 * Draws a pixel at the specified coordinates
 * @param x X coordinate
 * @param y Y coordinate
 * @param hexColor Color in RGB format
 */
void putPixel(uint64_t x, uint64_t y, uint32_t hexColor);

/**
 * Draws a character at the specified position
 * @param c Character to draw
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color in RGB format
 * @param font Font to use
 */
void drawChar(char c, int x, int y, uint32_t color, const bmp_font_t *font);

/**
 * Converts VGA color index to RGB
 * @param color VGA color index
 * @return RGB color value
 */
uint32_t vgaToRGB(uint8_t color);

/**
 * Draws multiple pixels at the specified coordinates
 * @param hexColor Color in RGB format
 * @param x X coordinate
 * @param y Y coordinate
 * @param mult Scale multiplier
 */
void putMultPixel(uint32_t hexColor, uint64_t x, uint64_t y, int mult);

/**
 * Draws a character using the default font
 * @param c Character to draw
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color in RGB format
 */
void drawCharDefault(char c, int x, int y, uint32_t color);

/**
 * Clears the entire screen with a color
 * @param color Color to fill the screen
 */
void clearScreen(uint32_t color);

/**
 * Draws a test pattern on screen
 */
void drawTestPattern(void);

/**
 * Draws a filled rectangle
 * @param x X coordinate
 * @param y Y coordinate
 * @param width Rectangle width
 * @param height Rectangle height
 * @param color Fill color
 */
void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
              uint32_t color);

/**
 * Adds a character to the screen buffer
 * @param c Character to add
 */
void screen_buffer_add_char(char c);

/**
 * Redraws the screen from the buffer
 */
void screen_buffer_redraw(void);

/**
 * Clears the screen buffer
 */
void screen_buffer_clear(void);

/**
 * Updates the FPS counter (call this every frame)
 */
void update_fps_counter(void);

/**
 * Gets the current FPS value
 * @return Current frames per second
 */
uint64_t get_current_fps(void);
