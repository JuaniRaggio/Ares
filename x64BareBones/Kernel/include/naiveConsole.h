#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

#define BLACK_WHITE 0x0f
#define GREEN_BLACK 0x20

void ncPrintNoColor(const char *string);
void ncPrint(const char *string, uint8_t color);
void ncPrintChar(char character, uint8_t color);
void ncNewline();
void ncPrintDec(uint64_t value);
void ncPrintHex(uint64_t value);
void ncPrintBin(uint64_t value);
void ncPrintBase(uint64_t value, uint32_t base);
void ncClear();

#endif
