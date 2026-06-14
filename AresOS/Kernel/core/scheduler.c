#include <stddef.h>
#include <drivers/time.h>
#include <process.h>
#include <scheduler.h>

#define SHELL_INDEX 0

extern uint8_t kernel_stack_top[];
extern char tss64[];

uint64_t current_kernel_stack;

static int current_index;
static uint64_t remaining_quantum;

static void set_tss_rsp0(uint64_t rsp0) {
        *(uint64_t *)((uint64_t)tss64 + 4) = rsp0;
}

static uint64_t kernel_stack_top_of(int index, pcb_t *pcb) {
        if (index == SHELL_INDEX)
                return (uint64_t)kernel_stack_top;
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
                if (pcb != NULL && pcb->state == PROCESS_READY)
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

        uint64_t kstack_top = kernel_stack_top_of(next_index, next);
        current_kernel_stack = kstack_top;
        set_tss_rsp0(kstack_top);
}

static void reap_if_zombie(pcb_t *process) {
        /* Reap only orphan zombies: if a process is waiting on this one, let
         * the waiter reap it so it reads a valid exit code (avoids the double
         * reap between the scheduler and process_wait). */
        if (process->state == PROCESS_ZOMBIE &&
            !process_has_waiter(process->pid)) {
                process_free_resources(process->pid);
                process->state = PROCESS_DEAD;
        }
}

static void demote_to_ready(pcb_t *process) {
        if (process->state == PROCESS_RUNNING)
                process->state = PROCESS_READY;
}

static int try_continue_current(pcb_t *current, uint64_t current_rsp) {
        if (current != NULL && current->state == PROCESS_READY) {
                current->state    = PROCESS_RUNNING;
                remaining_quantum = current->priority;
                return 1;
        }
        return 0;
}

uint64_t schedule(uint64_t current_rsp) {
        timer_handler();

        pcb_t *current = process_get_by_index(current_index);

        if (current != NULL) {
                current->rsp = current_rsp;
                reap_if_zombie(current);
                demote_to_ready(current);
        }

        if (remaining_quantum > 0 && current != NULL &&
            current->state == PROCESS_READY) {
                remaining_quantum--;
                current->state = PROCESS_RUNNING;
                return current_rsp;
        }

        int next = pick_next_ready();

        if (next < 0) {
                if (try_continue_current(current, current_rsp))
                        return current_rsp;
                return current_rsp;
        }

        switch_to(next);
        return process_get_by_index(next)->rsp;
}
