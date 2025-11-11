#pragma once
#include <stdint.h>

/**
 * Plays a sound with given frequency (blocking)
 * @param frequency Frequency in Hz
 * @param duration_ms Duration in milliseconds
 */
void playSound(uint64_t frequency, uint64_t duration_ms);

/**
 * Plays a short beep sound (blocking)
 * @param frequency Frequency in Hz (typical: 440-880 Hz)
 */
void beep(uint64_t frequency);
