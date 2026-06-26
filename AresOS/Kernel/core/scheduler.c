#include "interrupts.h"
#include <stddef.h>
#include <drivers/time.h>
#include <process.h>
#include <scheduler.h>

#define SHELL_INDEX 0

/* pick_next_ready() returns a table index; this sentinel means "none ready". */
#define NO_READY_PROCESS (-1)

extern uint8_t kernel_stack_top[];
extern char tss64[];

uint64_t current_kernel_stack;

static int current_index;

static void set_tss_rsp0(uint64_t rsp0) {
        *(uint64_t *)((uint64_t)tss64 + 4) = rsp0;
}

static uint64_t kernel_stack_top_of(int index, pcb_t *pcb) {
        if (index == SHELL_INDEX)
                return (uint64_t)kernel_stack_top;
        return (uint64_t)pcb->kernel_stack_base + KERNEL_STACK_SIZE;
}

void scheduler_init(void) {
        current_index        = 0;
        current_kernel_stack = (uint64_t)kernel_stack_top;
        process_set_current_pid(0);
        set_tss_rsp0((uint64_t)kernel_stack_top);
}

/*
 * Priority drives BOTH which process runs next and how much CPU it gets. Each
 * round a process gets `priority` credits (sched_credits); the scheduler always
 * picks the READY process of HIGHEST priority that still has credit, breaking
 * ties round-robin. A higher-priority process therefore runs first and more
 * often, so priority is visible in the finish order even for very short jobs,
 * not only over long ones. It still cannot starve lower priorities: once it
 * spends its credits the scheduler drops to the next level, and credits refill
 * only after every ready process has spent theirs.
 */
static void refill_credits(void) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                pcb_t *pcb = process_get_by_index(i);
                if (pcb != NULL && pcb->state == PROCESS_READY)
                        pcb->sched_credits = pcb->priority;
        }
}

/* Highest-priority READY process with credit left; ties broken round-robin from
 * current_index so equal-priority processes share their level fairly. */
static int scan_ready_with_credit(void) {
        int best          = NO_READY_PROCESS;
        int best_priority = 0;
        for (int i = 1; i <= MAX_PROCESSES; i++) {
                int idx    = (current_index + i) % MAX_PROCESSES;
                pcb_t *pcb = process_get_by_index(idx);
                if (pcb != NULL && pcb->state == PROCESS_READY &&
                    pcb->sched_credits > 0 && pcb->priority > best_priority) {
                        best          = idx;
                        best_priority = pcb->priority;
                }
        }
        if (best != NO_READY_PROCESS)
                process_get_by_index(best)->sched_credits--;
        return best;
}

static int pick_next_ready(void) {
        int idx = scan_ready_with_credit();
        if (idx != NO_READY_PROCESS)
                return idx;
        /* Every ready process spent its credits: start a new round. */
        refill_credits();
        return scan_ready_with_credit();
}

static void switch_to(int next_index) {
        current_index = next_index;
        pcb_t *next   = process_get_by_index(next_index);
        next->state   = PROCESS_RUNNING;
        process_set_current_pid(next->pid);

        uint64_t kstack_top  = kernel_stack_top_of(next_index, next);
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

static uint64_t do_schedule(uint64_t current_rsp) {
        pcb_t *current = process_get_by_index(current_index);

        if (current != NULL) {
                current->rsp = current_rsp;
                if (current->fpu_area != NULL)
                        fpu_save(current->fpu_area);
                reap_if_zombie(current);
                demote_to_ready(current);
        }

        int next = pick_next_ready();

        if (next == NO_READY_PROCESS) {
                /* Nothing else is runnable: keep the current one if it still
                 * can, otherwise leave the context untouched. */
                if (current != NULL && current->state == PROCESS_READY)
                        current->state = PROCESS_RUNNING;
                return current_rsp;
        }

        switch_to(next);
        pcb_t *next_pcb = process_get_by_index(next);
        if (next_pcb->fpu_area != NULL)
                fpu_restore(next_pcb->fpu_area);
        return next_pcb->rsp;
}

/* Timer-driven entry: advance timekeeping, wake any sleepers whose deadline
 * passed, then reschedule. */
uint64_t schedule(uint64_t current_rsp) {
        timer_handler();
        process_wake_sleepers(get_time_ms());
        return do_schedule(current_rsp);
}

/* Cooperative-yield entry (_irq81Handler / _yield_now): reschedule now without
 * touching timekeeping or the PIC. */
uint64_t do_yield_switch(uint64_t current_rsp) {
        return do_schedule(current_rsp);
}
