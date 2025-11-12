#include <drivers/time.h>
#include <sound.h>
#include <stdint.h>

/* PC Speaker control ports */
#define PIT_CHANNEL2 0x42 /* Channel 2 data port (used for speaker) */
#define PIT_COMMAND 0x43  /* Mode/Command register */
#define SPEAKER_PORT 0x61 /* PC Speaker control port */

/* PIT base frequency */
#define PIT_FREQUENCY 1193182

/**
 * Enables the PC Speaker hardware
 */
static void enable_speaker(void) {
        uint8_t tmp = inb(SPEAKER_PORT);
        outb(SPEAKER_PORT, tmp | 3); /* Set bits 0 and 1 to enable speaker */
}

/**
 * Disables the PC Speaker hardware
 */
static void disable_speaker(void) {
        uint8_t tmp = inb(SPEAKER_PORT);
        outb(SPEAKER_PORT, tmp & 0xFC);
}

/**
 * Sets the PC Speaker frequency
 * @param frequency Frequency in Hz
 */
static void set_speaker_frequency(uint64_t frequency) {
        if (frequency == 0)
                return;

        uint32_t divisor = PIT_FREQUENCY / frequency;

        /* Configure PIT channel 2 for square wave */
        outb(PIT_COMMAND,
             0xB6); /* 10110110: Channel 2, lobyte/hibyte, square wave */
        outb(PIT_CHANNEL2, (uint8_t)(divisor & 0xFF));
        outb(PIT_CHANNEL2, (uint8_t)((divisor >> 8) & 0xFF));
}

/**
 * Plays a sound with given frequency for specified duration (blocking)
 * @param frequency Frequency in Hz
 * @param duration_ms Duration in milliseconds
 */
void playSound(uint64_t frequency, uint64_t duration_ms) {
        if (frequency == 0 || duration_ms == 0) {
                return;
        }

        set_speaker_frequency(frequency);
        enable_speaker();

        /* Wait for the specified duration */
        uint64_t start_ms = get_time_ms();
        uint64_t end_ms   = start_ms + duration_ms;
        while (get_time_ms() < end_ms)
                ;

        disable_speaker();
}

/**
 * Plays a short beep sound (blocking)
 * @param frequency Frequency in Hz (typical: 440-880 Hz)
 */
void beep(uint64_t frequency) {
        playSound(frequency, 100); /* 100ms beep */
}
