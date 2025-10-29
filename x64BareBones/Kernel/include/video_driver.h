
#ifndef VIDEO_H
#define VIDEO_H


#include <fontManager.h>
#include <font_ubuntu_mono.h>
#include <stddef.h>
#include <colors.h>
#include <font.h>
#include <naiveConsole.h>
#include <stdint.h>

// ----------------------
// DIRECCIONES DE MEMORIA
// ----------------------
#define VIDEO_ADDR_GFX 0x000A0000
#define VIDEO_ADDR_TEXT 0xB8000

// ----------------------
// RESOLUCIÓN POR DEFECTO
// ----------------------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// ----------------------
// MODO TEXTO
// ----------------------
#define TEXT_WIDTH 80
#define TEXT_HEIGHT 25

// ----------------------
// VARIABLES GLOBALES
// ----------------------
extern uint8_t videoMode;      // 0 = texto, 1 = gráfico
extern uint32_t *framebuffer;  // dirección base framebuffer gráfico
extern uint8_t *videoTextBase; // dirección base modo texto
extern uint8_t *currentVideo;  // puntero actual en modo texto
extern int gfxCursorX, gfxCursorY;

// Información de pantalla
extern uint32_t screenWidth;
extern uint32_t screenHeight;
extern uint8_t bytesPerPixel;

// ----------------------
// FUNCIONES DE VIDEO
// ----------------------
void video_init(void);
void putPixel(uint64_t x, uint64_t y, uint32_t hexColor);
void drawChar(char c, int x, int y, uint32_t color, const bmp_font_t *font);
uint32_t vgaToRGB(uint8_t color);

//TESTING
static void putMultPixel(uint32_t hexColor, uint64_t x, uint64_t y, int mult);

void drawCharDefault(char c, int x, int y, uint32_t color);

// ----------------------
// FUNCIONES EXTRA (debug)
// ----------------------
void clearScreen(uint32_t color);
void drawTestPattern(void);

#endif
