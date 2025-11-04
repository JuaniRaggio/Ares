#include <drivers/time.h>

static uint64_t ticks = 0;

void timer_handler() {
        ticks++;
}

uint64_t ticks_elapsed() {
        return ticks;
}

uint64_t seconds_elapsed() {
        return ticks / 18;
}

void sleep(int seconds) {
        int initial_seconds = seconds_elapsed();
        while (seconds_elapsed() - initial_seconds < seconds)
                ;
}
