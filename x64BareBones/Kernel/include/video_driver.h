#ifndef __VIDEO_DRIVER_H__
#define __VIDEO_DRIVER_H__

#include <naiveConsole.h>
#include <stdint.h>

// Highest 3 bits -> Background (in this case 2 -> Green)
// Lowest 4 bits -> Foreground (in this case 0 -> Black)
// #define BLACK_WHITE 0x0f
// #define GREEN_BLACK 0x20

void printLn(const char *str, uint8_t color);

#endif
