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

static uint64_t kernel_stack_top_of(pcb_t *pcb) {
        return (uint64_t)pcb->kernel_stack_base + PROCESS_STACK_SIZE;
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
        for (int i = 1; i <= MAX_PROCESSES; i++) {
                int idx = (current_index + i) % MAX_PROCESSES;
                pcb_t *pcb = process_get(idx);
                if (pcb != (void *)0 && pcb->state == PROCESS_READY)
                        return idx;
        }
        return -1;
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
        pcb_t *current = process_get_current();
        current->rsp = current_rsp;
        if (current->state == PROCESS_RUNNING)
                current->state = PROCESS_READY;

        if (remaining_quantum > 0 && current->state == PROCESS_READY) {
                remaining_quantum--;
                current->state = PROCESS_RUNNING;
                return current_rsp;
        }

        int next = pick_next_ready();

        if (next < 0) {
                if (current_is_runnable()) {
                        current->state = PROCESS_RUNNING;
                        remaining_quantum = current->priority;
                        return current_rsp;
                }
                return current_rsp;
        }

        if (next != current_index) {
                switch_to(next);
                return process_get(next)->rsp;
        }

        current->state = PROCESS_RUNNING;
        remaining_quantum = current->priority;
        return current_rsp;
}
