#ifndef LIB_H
#define LIB_H

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
 * Gets the CPU vendor string
 * @param result Buffer to store the vendor string
 * @return Pointer to result buffer
 */
char *cpuVendor(char *result);

/**
 * Gets the current hour from RTC
 * @return Current hour (0-23)
 */
uint8_t get_current_hour(void);

/**
 * Gets the current minutes from RTC
 * @return Current minutes (0-59)
 */
uint8_t get_current_minutes(void);

/**
 * Gets the current seconds from RTC
 * @return Current seconds (0-59)
 */
uint8_t get_current_seconds(void);

/**
 * Gets the complete current time from RTC
 * @return Time structure with hours, minutes, and seconds
 */
s_time get_current_time(void);

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

#endif
