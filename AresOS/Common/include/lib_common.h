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

/**
 * Fills memory with a constant byte
 */
void *memset(void *destination, int32_t character, uint64_t length);

/**
 * Copies memory from source to destination
 */
void *memcpy(void *destination, const void *source, uint64_t length);

/**
 * Compares two strings
 */
int strcmp(const char *s1, const char *s2);

/**
 * Compares up to n characters of two strings
 */
int strncmp(const char *s1, const char *s2, uint64_t n);

/**
 * Calculates the length of a string
 */
uint64_t strlen(const char *s);

/**
 * Copies a string
 */
char *strcpy(char *dest, const char *src);

/**
 * Copies up to n characters of a string
 */
char *strncpy(char *dest, const char *src, uint64_t n);

static inline void minimize(uint64_t *to_minimize, uint64_t value) {
        *to_minimize = *to_minimize < value ? *to_minimize : value;
}

static inline void maximize(uint64_t *to_maximize, uint64_t value) {
        *to_maximize = *to_maximize > value ? *to_maximize : value;
}
