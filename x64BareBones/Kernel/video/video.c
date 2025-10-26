
#include <fontManager.h>
#include <font_ubuntu_mono.h>
#include <stddef.h>
#include <video.h>

// ------------------------------------------------------------
// Estructura VBE (inicializada por Pure64 en 0x5C00)
// ------------------------------------------------------------
typedef struct __attribute__((packed)) {
        uint16_t attributes;
        uint8_t window_a;
        uint8_t window_b;
        uint16_t granularity;
        uint16_t window_size;
        uint16_t segment_a;
        uint16_t segment_b;
        uint32_t win_func_ptr;
        uint16_t pitch;
        uint16_t width;
        uint16_t height;
        uint8_t w_char;
        uint8_t y_char;
        uint8_t planes;
        uint8_t bpp;
        uint8_t banks;
        uint8_t memory_model;
        uint8_t bank_size;
        uint8_t image_pages;
        uint8_t reserved0;
        uint8_t red_mask;
        uint8_t red_position;
        uint8_t green_mask;
        uint8_t green_position;
        uint8_t blue_mask;
        uint8_t blue_position;
        uint8_t reserved_mask;
        uint8_t reserved_position;
        uint8_t direct_color_attributes;
        uint32_t framebuffer;
        uint32_t off_screen_mem_off;
        uint16_t off_screen_mem_size;
        uint8_t reserved1[206];
} __attribute__((packed)) vbe_mode_info_t;

// ------------------------------------------------------------
// Variables globales
// ------------------------------------------------------------
uint8_t videoMode      = 0;
uint32_t *framebuffer  = 0;
uint8_t *videoTextBase = (uint8_t *)VIDEO_ADDR_TEXT;
uint8_t *currentVideo  = (uint8_t *)VIDEO_ADDR_TEXT;

int gfxCursorX = 0, gfxCursorY = 0;
uint32_t screenWidth  = SCREEN_WIDTH;
uint32_t screenHeight = SCREEN_HEIGHT;
uint8_t bytesPerPixel = 4;

// ------------------------------------------------------------
// Inicialización del video
// ------------------------------------------------------------
void video_init(void) {
        vbe_mode_info_t *vbe = (vbe_mode_info_t *)0x0000000000005C00;

        if (vbe->framebuffer != 0 && vbe->width > 0 && vbe->height > 0) {
                framebuffer   = (uint32_t *)(uint64_t)vbe->framebuffer;
                screenWidth   = vbe->width;
                screenHeight  = vbe->height;
                bytesPerPixel = vbe->bpp / 8;
                videoMode     = 1;
        } else {
                framebuffer = (uint32_t *)VIDEO_ADDR_GFX;
                videoMode   = 0;
        }

        registerFont(&font_ubuntu_mono);
        setFontByName("Ubuntu Mono");

        clearScreen(0x000000);
}

// ------------------------------------------------------------
// Dibuja un píxel en coordenadas (x, y)
// ------------------------------------------------------------
void drawPixel(int x, int y, uint32_t color) {
        if (x < 0 || y < 0 || x >= (int)screenWidth || y >= (int)screenHeight)
                return;

        vbe_mode_info_t *vbe = (vbe_mode_info_t *)0x0000000000005C00;
        uint32_t pitch       = vbe->pitch;

        uint8_t *base  = (uint8_t *)(uint64_t)vbe->framebuffer;
        uint8_t *pixel = base + y * pitch + x * bytesPerPixel;

        // QEMU/Pure64 usa BGR
        pixel[0] = (color >> 0) & 0xFF;  // Blue
        pixel[1] = (color >> 8) & 0xFF;  // Green
        pixel[2] = (color >> 16) & 0xFF; // Red
}

// ------------------------------------------------------------
// Dibuja un carácter usando la fuente bitmap
// ------------------------------------------------------------
void drawChar(char c, int x, int y, uint32_t color, bmp_font_t *font) {
        if (font == NULL)
                return;

        const unsigned char *glyph = font->bitmap[(unsigned char)c];
        for (int row = 0; row < font->height; row++) {
                unsigned char bits = glyph[row];
                for (int col = 0; col < font->width; col++) {
                        if (bits & (1 << col)) {
                                drawPixel(x + col, y + row, color);
                        }
                }
        }
}

// ------------------------------------------------------------
// Conversión de color VGA (4 bits) a RGB
// ------------------------------------------------------------
uint32_t vgaToRGB(uint8_t color) {
        switch (color & 0x0F) {
        case 0x00:
                return 0x000000; // negro
        case 0x01:
                return 0x0000FF; // azul
        case 0x02:
                return 0x00FF00; // verde
        case 0x04:
                return 0xFF0000; // rojo
        case 0x07:
                return 0xC0C0C0; // gris
        case 0x0F:
                return 0xFFFFFF; // blanco
        default:
                return 0xAAAAAA;
        }
}

// ------------------------------------------------------------
// Limpia toda la pantalla con un color
// ------------------------------------------------------------
void clearScreen(uint32_t color) {
        vbe_mode_info_t *vbe = (vbe_mode_info_t *)0x0000000000005C00;
        uint32_t pitch       = vbe->pitch;

        uint8_t *base = (uint8_t *)(uint64_t)vbe->framebuffer;

        for (uint32_t y = 0; y < screenHeight; y++) {
                for (uint32_t x = 0; x < screenWidth; x++) {
                        uint8_t *pixel = base + y * pitch + x * bytesPerPixel;
                        pixel[0]       = (color >> 0) & 0xFF;
                        pixel[1]       = (color >> 8) & 0xFF;
                        pixel[2]       = (color >> 16) & 0xFF;
                }
        }
}

// ------------------------------------------------------------
// Dibuja un patrón de prueba (degradé)
// ------------------------------------------------------------
void drawTestPattern(void) {
        vbe_mode_info_t *vbe = (vbe_mode_info_t *)0x0000000000005C00;
        uint32_t pitch       = vbe->pitch;
        uint8_t *base        = (uint8_t *)(uint64_t)vbe->framebuffer;

        for (uint32_t y = 0; y < screenHeight; y++) {
                for (uint32_t x = 0; x < screenWidth; x++) {
                        uint32_t color = ((x * 255 / screenWidth) << 16) |
                                         ((y * 255 / screenHeight) << 8);
                        uint8_t *pixel = base + y * pitch + x * bytesPerPixel;
                        pixel[0]       = (color >> 0) & 0xFF;
                        pixel[1]       = (color >> 8) & 0xFF;
                        pixel[2]       = (color >> 16) & 0xFF;
                }
        }
}
