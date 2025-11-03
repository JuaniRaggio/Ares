// lib.h
#pragma once

#include <stdint.h>

typedef struct {
        uint8_t hours;
        uint8_t minutes;
} s_time;

// Memoria
void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);

// CPU info
char *cpuVendor(char *result);

// Tiempo
uint8_t get_current_hour(void);
uint8_t get_current_minutes(void);
s_time get_current_time(void);

// Funciones de string
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, uint64_t n);
uint64_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, uint64_t n);

// Number to string conversion
/**
 * Converts a signed integer to a string representation in the specified base.
 * @param value The integer value to convert
 * @param str Buffer to store the resulting string (must be large enough)
 * @param base Numeric base for conversion (2-36). Negative values only supported in base 10
 * @return Pointer to the resulting string (same as str parameter)
 */
char *itoa(int value, char *str, int base);

/**
 * Converts an unsigned 64-bit integer to a string representation in the specified base.
 * @param value The unsigned 64-bit integer value to convert
 * @param str Buffer to store the resulting string (must be large enough)
 * @param base Numeric base for conversion (2-36)
 * @return Pointer to the resulting string (same as str parameter)
 */
char *utoa(uint64_t value, char *str, int base);

