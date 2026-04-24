#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <lib_common.h> // Renaming common lib.h to lib_common.h to avoid collision

/**
 * Gets the CPU vendor string
 */
char *cpuVendor(char *result);

/**
 * Gets the current hour from RTC
 */
uint8_t get_current_hour(void);

/**
 * Gets the current minutes from RTC
 */
uint8_t get_current_minutes(void);

/**
 * Gets the current seconds from RTC
 */
uint8_t get_current_seconds(void);

/**
 * Gets the complete current time from RTC
 */
s_time get_current_time(void);

/**
 * Reads the CPU Time Stamp Counter
 */
uint64_t read_tsc(void);

/**
 * Writes a byte to an I/O port
 */
void outb(uint16_t port, uint8_t value);

/**
 * Reads a byte from an I/O port
 */
uint8_t inb(uint16_t port);

#endif
