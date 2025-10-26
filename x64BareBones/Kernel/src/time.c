#include <time.h>

static unsigned long ticks = 0;

void timer_handler() {
        ticks++;
}

int ticks_elapsed() {
        return ticks;
}

int seconds_elapsed() {
        return ticks / 18;
}

void sleep(int seconds) {
        int initial_seconds = seconds_elapsed();
        while (seconds_elapsed() - initial_seconds < seconds)
                ;
}
