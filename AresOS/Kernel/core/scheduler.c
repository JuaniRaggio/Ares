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
        process_set_current_pid(0);
        set_tss_rsp0((uint64_t)kernel_stack_top);
}

void scheduler_yield(void) {
        remaining_quantum = 0;
}

static int pick_next_ready(void) {
        for (int i = 1; i <= MAX_PROCESSES; i++) {
                int idx    = (current_index + i) % MAX_PROCESSES;
                pcb_t *pcb = process_get_by_index(idx);
                if (pcb != (void *)0 && pcb->state == PROCESS_READY)
                        return idx;
        }
        return -1;
}

static void switch_to(int next_index) {
        current_index     = next_index;
        pcb_t *next       = process_get_by_index(next_index);
        next->state       = PROCESS_RUNNING;
        remaining_quantum = next->priority;
        process_set_current_pid(next->pid);

        uint64_t kstack_top = kernel_stack_top_of(next);
        /* If shell (index 0), it uses static boot stack for now */
        if (next_index == 0)
                kstack_top = (uint64_t)kernel_stack_top;

        current_kernel_stack = kstack_top;
        set_tss_rsp0(kstack_top);
}

uint64_t schedule(uint64_t current_rsp) {
        timer_handler();

        pcb_t *current = process_get_by_index(current_index);

        if (current != (void *)0) {
                current->rsp = current_rsp;

                if (current->state == PROCESS_ZOMBIE) {
                        process_free_resources(current->pid);
                        current->state = PROCESS_DEAD;
                } else if (current->state == PROCESS_RUNNING) {
                        current->state = PROCESS_READY;
                }
        }

        if (remaining_quantum > 0 && current != (void *)0 &&
            current->state == PROCESS_READY) {
                remaining_quantum--;
                current->state = PROCESS_RUNNING;
                return current_rsp;
        }

        int next = pick_next_ready();

        if (next < 0) {
                /* No other ready process, keep running current if it's still
                 * runnable */
                if (current != (void *)0 && current->state == PROCESS_READY) {
                        current->state    = PROCESS_RUNNING;
                        remaining_quantum = current->priority;
                        return current_rsp;
                }
                /* If nothing is ready (shouldn't happen with idle/shell), just
                 * return */
                return current_rsp;
        }

        switch_to(next);
        return process_get_by_index(next)->rsp;
}
