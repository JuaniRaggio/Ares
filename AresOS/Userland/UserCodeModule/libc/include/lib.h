#pragma once

#include <lib_common.h>

/**
 * Converts a signed integer to a string representation in the specified base.
 */
char *itoa(int value, char *str, int base);

/**
 * Converts an unsigned 64-bit integer to a string representation in the
 * specified base.
 */
char *utoa(uint64_t value, char *str, int base);

/**
 * Converts a string to an unsigned long integer
 */
uint64_t strtoul(const char *str, char **endptr, int base);
