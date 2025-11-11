#include <drivers/time.h>

/* PIT timer tick counter (IRQ 0 fires at TICKS_PER_SECOND Hz) */
static uint64_t tick_counter  = 0;
static uint64_t start_seconds = 0;
static uint64_t start_minutes = 0;
static uint64_t start_hours   = 0;

/* PIT ports */
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43

/* PIT frequency configuration */
#define PIT_FREQUENCY 1193182
#define TICKS_PER_SECOND 18 /* ~18.2 Hz (55ms per tick) */
#define PIT_DIVISOR 65536   /* Max divisor for ~18.2 Hz */

void timer_init(void) {
        /* Capture initial time */
        start_seconds = get_current_seconds();
        start_minutes = get_current_minutes();
        start_hours   = get_current_hour();
        tick_counter  = 0;

        /* Initialize PIT (Programmable Interval Timer)
         * Command byte: 0x36
         * - Channel 0 (bits 7-6: 00)
         * - Access mode: lobyte/hibyte (bits 5-4: 11)
         * - Mode 3: square wave (bits 3-1: 011)
         * - Binary mode (bit 0: 0)
         */
        outb(PIT_COMMAND, 0x36);

        /* Set divisor for ~18.2 Hz (55ms per tick)
         * Divisor 65536 (0x10000) is written as 0x00 0x00 */
        outb(PIT_CHANNEL0, 0x00); /* Low byte */
        outb(PIT_CHANNEL0, 0x00); /* High byte */
}

uint64_t seconds_elapsed() {
        uint64_t curr_seconds = get_current_seconds();
        uint64_t curr_minutes = get_current_minutes();
        uint64_t curr_hours   = get_current_hour();
        return curr_hours * 3600 + curr_minutes * 60 + curr_seconds;
}

void timer_handler() {
        tick_counter++;
}

uint64_t ticks_elapsed() {
        return tick_counter;
}

void sleep(int seconds) {
        int initial_seconds = seconds_elapsed();
        while (seconds_elapsed() - initial_seconds < seconds)
                ;
}

uint64_t get_time_ms() {
        return (tick_counter * 549) / 10;
}
