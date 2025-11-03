#include <drivers/video_driver.h>

struct vbe_mode_info_structure {
        uint16_t attributes; // deprecated, only bit 7 should be of interest to
        // you, and it indicates the mode supports a linear
        // frame buffer.
        uint8_t window_a;     // deprecated
        uint8_t window_b;     // deprecated
        uint16_t granularity; // deprecated; used while calculating bank numbers
        uint16_t window_size;
        uint16_t segment_a;
        uint16_t segment_b;
        uint32_t win_func_ptr; // deprecated; used to switch banks from
        // protected mode without returning to real mode
        uint16_t pitch;  // number of bytes per horizontal line
        uint16_t width;  // width in pixels
        uint16_t height; // height in pixels
        uint8_t w_char;  // unused...
        uint8_t y_char;  // ...
        uint8_t planes;
        uint8_t bpp;   // bits per pixel in this mode
        uint8_t banks; // deprecated; total number of banks in this mode
        uint8_t memory_model;
        uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB
        // but may be 16 KB...
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

        uint32_t framebuffer; // physical address of the linear frame buffer;
        // write here to draw to the screen
        uint32_t off_screen_mem_off;
        uint16_t off_screen_mem_size; // size of memory in the framebuffer but
        // not being displayed on the screen
        uint8_t reserved1[206];
} __attribute__((packed));

typedef struct vbe_mode_info_structure *VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr)0x0000000000005C00;

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
        if (VBE_mode_info->framebuffer != 0 && VBE_mode_info->width > 0 &&
            VBE_mode_info->height > 0) {
                framebuffer  = (uint32_t *)(uint64_t)VBE_mode_info->framebuffer;
                screenWidth  = VBE_mode_info->width;
                screenHeight = VBE_mode_info->height;
                bytesPerPixel = VBE_mode_info->bpp / 8;
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
void putPixel(uint64_t x, uint64_t y, uint32_t hexColor) {
        uint8_t *framebuffer = (uint8_t *)VBE_mode_info->framebuffer;
        uint64_t offset =
            (x * ((VBE_mode_info->bpp) / 8)) + (y * VBE_mode_info->pitch);
        framebuffer[offset]     = (hexColor) & 0xFF;
        framebuffer[offset + 1] = (hexColor >> 8) & 0xFF;
        framebuffer[offset + 2] = (hexColor >> 16) & 0xFF;
}

void putMultPixel(uint32_t hexColor, uint64_t x, uint64_t y, int mult) {
        for (int i = 0; i < mult; i++) {
                for (int j = 0; j < mult; j++) {
                        putPixel(hexColor, x + i, y + j);
                }
        }
}

// ------------------------------------------------------------
// Dibuja un carácter usando la fuente bitmap
// ------------------------------------------------------------
void drawChar(char c, int x, int y, uint32_t color, const bmp_font_t *font) {
        if (font == NULL)
                return;

        const unsigned char *glyph = font->bitmap[(unsigned char)c];
        for (int row = 0; row < font->height; row++) {
                unsigned char bits = glyph[row];
                for (int col = 0; col < font->width; col++) {
                        if (bits & (1 << col)) {
                                putPixel(x + col, y + row, color);
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
        uint32_t pitch = VBE_mode_info->pitch;

        uint8_t *base = (uint8_t *)(uint64_t)VBE_mode_info->framebuffer;

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
        uint32_t pitch = VBE_mode_info->pitch;
        uint8_t *base  = (uint8_t *)(uint64_t)VBE_mode_info->framebuffer;

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

void drawCharDefault(char c, int x, int y, uint32_t color) {
        drawChar(c, x, y, color, getFont());
}
