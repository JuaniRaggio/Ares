#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

/**
 * Initializes the timer subsystem
 */
void timer_init(void);

/**
 * Timer interrupt handler
 */
void timer_handler();

/**
 * Gets the number of timer ticks elapsed since boot
 * @return Number of ticks
 */
uint64_t ticks_elapsed();

/**
 * Gets the number of seconds elapsed since boot
 * @return Number of seconds
 */
uint64_t seconds_elapsed();

/**
 * Sleeps for a specified number of seconds
 * @param seconds Number of seconds to sleep
 */
void sleep(int seconds);

#endif
