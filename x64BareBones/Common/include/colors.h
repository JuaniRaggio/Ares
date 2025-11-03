/* colors.h - Definiciones de colores compartidas
 * CONTRATO entre kernel y userland
 * Solo constantes - sin funciones
 */
#ifndef COLORS_H
#define COLORS_H

// Colores RGB (32-bit)
#define BLACK 0x000000
#define WHITE 0xFFFFFF
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define YELLOW 0xFFFF00
#define CYAN 0x00FFFF
#define MAGENTA 0xFF00FF
#define GRAY 0x808080

// Colores VGA (8-bit)
#define VGA_BLACK 0x00
#define VGA_BLUE 0x01
#define VGA_GREEN 0x02
#define VGA_CYAN 0x03
#define VGA_RED 0x04
#define VGA_MAGENTA 0x05
#define VGA_BROWN 0x06
#define VGA_GRAY 0x07
#define VGA_WHITE 0x0F

#endif /* COLORS_H */
