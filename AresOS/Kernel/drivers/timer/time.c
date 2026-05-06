#include <drivers/time.h>

static volatile uint64_t tick_counter = 0;
static uint64_t start_seconds         = 0;
static uint64_t start_minutes         = 0;
static uint64_t start_hours           = 0;

#define PIT_CHANNEL0     0x40
#define PIT_COMMAND      0x43
#define PIT_FREQUENCY    1193182
#define TICKS_PER_SECOND 18
#define PIT_DIVISOR      65536

#define PIT_SQUARE_WAVE_CH0 0x36
#define PIT_MAX_DIVISOR_LO  0x00
#define PIT_MAX_DIVISOR_HI  0x00

static void capture_initial_time(void) {
        start_seconds = get_current_seconds();
        start_minutes = get_current_minutes();
        start_hours   = get_current_hour();
        tick_counter  = 0;
}

static void configure_pit(void) {
        outb(PIT_COMMAND, PIT_SQUARE_WAVE_CH0);
        outb(PIT_CHANNEL0, PIT_MAX_DIVISOR_LO);
        outb(PIT_CHANNEL0, PIT_MAX_DIVISOR_HI);
}

void timer_init(void) {
        capture_initial_time();
        configure_pit();
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
