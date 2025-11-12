#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <colors.h>
#include <fontManager.h>
#include <stdint.h>
#include <video_driver.h>

/**
 * Prints a string followed by a newline
 * @param str String to print
 * @param color Text color
 */
void printLn(const char *str, const uint8_t color);

/**
 * Prints a string (legacy version with single parameter)
 * @param string String to print
 */
void ncPrintOld(const char *string);

/**
 * Prints a string with color
 * @param string String to print
 * @param color Text color
 */
void ncPrint(const char *string, uint8_t color);

/**
 * Prints a single character with color
 * @param c Character to print
 * @param color Text color
 */
void ncPrintChar(char c, uint8_t color);

/**
 * Prints a character in text mode
 * @param character Character to print
 * @param color Text color
 */
void ncPrintCharText(char character, uint8_t color);

/**
 * Prints a string in text mode
 * @param string String to print
 * @param color Text color
 */
void ncPrintText(const char *string, uint8_t color);

/**
 * Prints a string in graphics mode
 * @param string String to print
 * @param color Text color
 */
void ncPrintVideo(const char *string, uint8_t color);

/**
 * Prints a newline
 */
void ncNewline(void);

/**
 * Prints a decimal number
 * @param value Value to print
 */
void ncPrintDec(uint64_t value);

/**
 * Prints a hexadecimal number
 * @param value Value to print
 */
void ncPrintHex(uint64_t value);

/**
 * Prints a binary number
 * @param value Value to print
 */
void ncPrintBin(uint64_t value);

/**
 * Prints a number in a given base
 * @param value Value to print
 * @param base Numeric base (2-36)
 */
void ncPrintBase(uint64_t value, uint32_t base);

/**
 * Clears the console
 */
void ncClear(void);

/**
 * Prints a single character with RGB color
 * @param c Character to print
 * @param rgb RGB color value (0xRRGGBB)
 */
void ncPrintCharRGB(char c, uint32_t rgb);

#endif
