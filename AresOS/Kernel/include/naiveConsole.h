// naiveConsole.h

#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <colors.h>
#include <fontManager.h>
#include <stdint.h>
#include <video_driver.h>

void printLn(const char *str, const uint8_t color);

// temp. solution for older versions where ncPrint is declared with only one
// arg.
void ncPrintOld(const char *string);

void ncPrint(const char *string, uint8_t color);
void ncPrintChar(char c, uint8_t color);
void ncPrintCharText(char character, uint8_t color);
void ncPrintText(const char *string, uint8_t color);
void ncPrintVideo(const char *string, uint8_t color);
void ncNewline(void);

void ncPrintDec(uint64_t value);
void ncPrintHex(uint64_t value);
void ncPrintBin(uint64_t value);
void ncPrintBase(uint64_t value, uint32_t base);
void ncClear(void);

#endif
