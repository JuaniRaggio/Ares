#pragma once

#include <colors.h>
#include <font.h>
#include <fontManager.h>
#include <font_ubuntu_mono.h>
#include <stdint.h>

#define NULL ((void *)0)

// -------------------------
// Memory addresses
// -------------------------
#define VIDEO_ADDR_GFX 0x000A0000
#define VIDEO_ADDR_TEXT 0xB8000

// -------------------------
// Default resolution
// -------------------------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// -------------------------
// Text mode
// -------------------------
#define TEXT_WIDTH 80
#define TEXT_HEIGHT 25

// -------------------------
// Global variables (Kernel)
// -------------------------
extern uint8_t videoMode;      // 0 = texto, 1 = gráfico
extern uint32_t *framebuffer;  // dirección base framebuffer gráfico
extern uint8_t *videoTextBase; // dirección base modo texto
extern uint8_t *currentVideo;  // puntero actual en modo texto
extern int gfxCursorX, gfxCursorY;

// Screen data
extern uint32_t screenWidth;
extern uint32_t screenHeight;
extern uint8_t bytesPerPixel;
extern uint8_t fontScale;
static uint8_t *limit = (uint8_t *)0xB8FA0; // video + width * height * 2 - 1

// -------------------------
// Video functions
// -------------------------
void video_init(void);
void putPixel(uint64_t x, uint64_t y, uint32_t hexColor);
void drawChar(char c, int x, int y, uint32_t color, const bmp_font_t *font);
uint32_t vgaToRGB(uint8_t color);

// Test
void putMultPixel(uint32_t hexColor, uint64_t x, uint64_t y, int mult);
void drawCharDefault(char c, int x, int y, uint32_t color);

// ----------------------
// EXTRAS
// ----------------------
void clearScreen(uint32_t color);
void drawTestPattern(void);
void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

// Screen buffer for redraw
void screen_buffer_add_char(char c);
void screen_buffer_redraw(void);
