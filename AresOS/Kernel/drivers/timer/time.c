#include <drivers/time.h>
#include <lib.h>

static uint64_t start_seconds = 0;
static uint64_t start_minutes = 0;
static uint64_t start_hours   = 0;

void timer_init(void) {
        /* Capture initial time from RTC */
        start_seconds = get_current_seconds();
        start_minutes = get_current_minutes();
        start_hours   = get_current_hour();
}

uint64_t seconds_elapsed() {
        /* Read current time from RTC and return total seconds since midnight */
        uint64_t curr_seconds = get_current_seconds();
        uint64_t curr_minutes = get_current_minutes();
        uint64_t curr_hours   = get_current_hour();
        return curr_hours * 3600 + curr_minutes * 60 + curr_seconds;
}

void timer_handler() {
}

uint64_t ticks_elapsed() {
        /* Approximate ticks as seconds * 18 for compatibility */
        return seconds_elapsed() * 18;
}

void sleep(int seconds) {
        int initial_seconds = seconds_elapsed();
        while (seconds_elapsed() - initial_seconds < seconds)
                ;
}
