#include <drivers/time.h>
#include <process.h>
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
        outb(SPEAKER_PORT, tmp | 3);
}

/**
 * Disables the PC Speaker hardware
 */
static void disable_speaker(void) {
        uint8_t tmp = inb(SPEAKER_PORT);
        outb(SPEAKER_PORT, tmp & 0xFC);
}

#define PIT_SQUARE_WAVE_CH2 0xB6

static void set_speaker_frequency(uint64_t frequency) {
        if (frequency == 0)
                return;

        uint32_t divisor = PIT_FREQUENCY / frequency;

        outb(PIT_COMMAND, PIT_SQUARE_WAVE_CH2);
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
        process_sleep_ms(duration_ms); /* block, do not busy-wait */
        disable_speaker();
}

/**
 * Plays a short beep sound (blocking)
 * @param frequency Frequency in Hz (typical: 440-880 Hz)
 */
#define BEEP_DURATION_MS 100

void beep(uint64_t frequency) {
        playSound(frequency, BEEP_DURATION_MS);
}
