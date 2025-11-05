#pragma once

#include <lib.h>
#include <stdarg.h>
#include <stdint.h>
#include <syscalls.h>

/**
 * Prints formatted output to stdout
 * @param format Format string
 * @return Number of characters printed
 */
int printf(const char *format, ...);

/**
 * Writes a character to stdout
 * @param c Character to write
 * @return Character written
 */
int putchar(int c);

/**
 * Writes a string followed by newline to stdout
 * @param s String to write
 * @return Non-negative on success
 */
int puts(const char *s);

/**
 * Reads a character from stdin
 * @return Character read
 */
int getchar(void);

/**
 * Reads formatted input from stdin
 * @param fmt Format string
 * @return Number of items successfully read
 */
int scanf(char *fmt, ...);
