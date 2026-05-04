#include <drivers/time.h>
#include <process.h>
#include <scheduler.h>

extern uint8_t kernel_stack_top[];
extern char tss64[];

uint64_t current_kernel_stack;

static int current_index;
static uint64_t remaining_quantum;

static void set_tss_rsp0(uint64_t rsp0) {
        *(uint64_t *)((uint64_t)tss64 + 4) = rsp0;
}
void scheduler_init(void) {
}

static int pick_next_ready(void) {
}

static int current_is_runnable(void) {
}

static void switch_to(int next_index) {
}

uint64_t schedule(uint64_t current_rsp) {
}
