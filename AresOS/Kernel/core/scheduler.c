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
        current_index        = 0;
        remaining_quantum    = DEFAULT_PRIORITY;
        current_kernel_stack = (uint64_t)kernel_stack_top;
        set_tss_rsp0((uint64_t)kernel_stack_top);
}

void scheduler_yield(void) {
        remaining_quantum = 0;
}

static int pick_next_ready(void) {
}

static int current_is_runnable(void) {
        pcb_t *cur = process_get(current_index);
        return cur != (void *)0 &&
               (cur->state == PROCESS_READY || cur->state == PROCESS_RUNNING);
}

static void switch_to(int next_index) {
        current_index = next_index;
        pcb_t *next   = process_get(next_index);
        next->state   = PROCESS_RUNNING;
        remaining_quantum = next->priority;

        uint64_t kstack_top = kernel_stack_top_of(next);
        current_kernel_stack = kstack_top;
        set_tss_rsp0(kstack_top);
}

uint64_t schedule(uint64_t current_rsp) {
}
