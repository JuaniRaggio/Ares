#pragma once

#include <stdint.h>

/**
 * Time structure
 */
typedef struct {
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
} s_time;

static inline void minimize(uint64_t *to_minimize, uint64_t value) {
        *to_minimize = *to_minimize < value ? *to_minimize : value;
}

static inline void maximize(uint64_t *to_maximize, uint64_t value) {
        *to_maximize = *to_maximize > value ? *to_maximize : value;
}

/**
 * Fills memory with a constant byte
 * @param destination Pointer to memory to fill
 * @param character Byte value to fill with
 * @param length Number of bytes to fill
 * @return Pointer to destination
 */
void *memset(void *destination, int32_t character, uint64_t length);

/**
 * Copies memory from source to destination
 * @param destination Destination pointer
 * @param source Source pointer
 * @param length Number of bytes to copy
 * @return Pointer to destination
 */
void *memcpy(void *destination, const void *source, uint64_t length);

/**
 * Compares two strings
 * @param s1 First string
 * @param s2 Second string
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2
 */
int strcmp(const char *s1, const char *s2);

/**
 * Compares up to n characters of two strings
 * @param s1 First string
 * @param s2 Second string
 * @param n Maximum number of characters to compare
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2
 */
int strncmp(const char *s1, const char *s2, uint64_t n);

/**
 * Calculates the length of a string
 * @param s String to measure
 * @return Length of the string
 */
uint64_t strlen(const char *s);

/**
 * Copies a string
 * @param dest Destination buffer
 * @param src Source string
 * @return Pointer to destination
 */
char *strcpy(char *dest, const char *src);

/**
 * Copies up to n characters of a string
 * @param dest Destination buffer
 * @param src Source string
 * @param n Maximum number of characters to copy
 * @return Pointer to destination
 */
char *strncpy(char *dest, const char *src, uint64_t n);

/**
 * Converts a signed integer to a string representation in the specified base.
 * @param value The integer value to convert
 * @param str Buffer to store the resulting string (must be large enough)
 * @param base Numeric base for conversion (2-36). Negative values only
 * supported in base 10
 * @return Pointer to the resulting string (same as str parameter)
 */
char *itoa(int value, char *str, int base);

/**
 * Converts an unsigned 64-bit integer to a string representation in the
 * specified base.
 * @param value The unsigned 64-bit integer value to convert
 * @param str Buffer to store the resulting string (must be large enough)
 * @param base Numeric base for conversion (2-36)
 * @return Pointer to the resulting string (same as str parameter)
 */
char *utoa(uint64_t value, char *str, int base);

/**
 * Converts a string to an unsigned long integer
 * @param str String to convert
 * @param endptr Pointer to store the address of the first invalid character
 * @param base Numeric base (0 for auto-detect, 10 for decimal, 16 for hex)
 * @return Converted unsigned integer value
 */
uint64_t strtoul(const char *str, char **endptr, int base);
