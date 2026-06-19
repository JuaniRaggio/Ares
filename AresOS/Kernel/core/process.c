#include <stddef.h>
#include <drivers/time.h>
#include <interrupts.h>
#include <lib_common.h>
#include <memory_manager.h>
#include <pipe.h>
#include <process.h>
#include <semaphores.h>
#include <status_codes.h>
#include <stdint.h>

/* GDT layout puts user data (0x18) before user code (0x20) as SYSRET
 * requires; both selectors carry RPL=3 */
#define USER_CS 0x23
#define USER_SS 0x1B
#define RFLAGS_IF 0x202
#define SHELL_INDEX 0

static pcb_t process_table[MAX_PROCESSES];
static pid_t current_pid;
static pid_t next_pid_to_assign = 0;

/* Clean FPU state captured once at boot; copied into each new process so its
 * first fxrstor loads a valid MXCSR instead of garbage. 16-aligned for fxsave. */
static uint8_t fpu_template[FPU_AREA_SIZE] __attribute__((aligned(16)));

extern void scheduler_yield(void);

/* Allocate and initialize a process FPU save area from the clean template. */
static uint8_t *alloc_fpu_area(void) {
        uint8_t *area = (uint8_t *)mem_alloc(FPU_AREA_SIZE);
        if (area != NULL)
                memcpy(area, fpu_template, FPU_AREA_SIZE);
        return area;
}

void process_set_current_pid(pid_t pid) {
        current_pid = pid;
}

void process_free_resources(pid_t pid) {
        if (pid < 0)
                return;
        for (int i = 0; i < MAX_PROCESSES; i++) {
                if (process_table[i].pid == pid) {
                        pcb_t *pcb = &process_table[i];
                        if (pcb->kernel_stack_base != NULL) {
                                mem_free(pcb->kernel_stack_base);
                                pcb->kernel_stack_base = NULL;
                        }
                        if (pcb->user_stack_base != NULL) {
                                mem_free(pcb->user_stack_base);
                                pcb->user_stack_base = NULL;
                        }
                        if (pcb->argv_copy != NULL) {
                                mem_free(pcb->argv_copy);
                                pcb->argv_copy = NULL;
                        }
                        if (pcb->fpu_area != NULL) {
                                mem_free(pcb->fpu_area);
                                pcb->fpu_area = NULL;
                        }
                        /* Free any memory the process requested through the
                         * malloc syscall and never freed, so a killed process
                         * leaks nothing. */
                        user_alloc_node_t *head = &pcb->user_allocs;
                        user_alloc_node_t *node = head->next;
                        while (node != NULL && node != head) {
                                user_alloc_node_t *next = node->next;
                                mem_free(node);
                                node = next;
                        }
                        head->next = head->prev = head;
                        return;
                }
        }
}

/* Copies argv into a single kernel-owned block so the arguments survive
 * after the creator reuses or frees its own buffers (background processes). */
static char **clone_argv(uint64_t argc, char **argv) {
        if (argc == 0 || argv == NULL)
                return NULL;

        uint64_t total = (argc + 1) * sizeof(char *);
        for (uint64_t i = 0; i < argc; i++)
                total += strlen(argv[i]) + 1;

        char **copy = (char **)mem_alloc(total);
        if (copy == NULL)
                return NULL;

        char *strings = (char *)(copy + argc + 1);
        for (uint64_t i = 0; i < argc; i++) {
                copy[i] = strings;
                strcpy(strings, argv[i]);
                strings += strlen(argv[i]) + 1;
        }
        copy[argc] = NULL;
        return copy;
}

static void wake_waiters(pid_t dead_pid) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                if (process_table[i].state == PROCESS_BLOCKED &&
                    process_table[i].waiting_for == dead_pid) {
                        process_table[i].state = PROCESS_READY;
                        /* Keep waiting_for set: process_has_waiter relies on it
                         * so the scheduler does not reap this zombie before the
                         * waiter reads its exit code. process_wait clears it. */
                }
        }
}

/* Called when a process dies. Its still-living children are re-parented to the
 * shell (init-style) so they are never orphaned, and any zombie that no longer
 * has a waiter is reaped now: the scheduler only reaps the running process, so a
 * zombie that loses its waiter (its parent died before reaping it) would
 * otherwise leak its PCB and stacks forever. The running process is skipped; it
 * cannot free its own stack here and the scheduler reaps it on the next tick. */
static void reparent_and_reap_orphans(pid_t dead_pid) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                pcb_t *child = &process_table[i];
                if (child->parent_pid == dead_pid && child->pid != dead_pid &&
                    child->state != PROCESS_DEAD)
                        child->parent_pid = SHELL_PID;
        }
        for (int i = 0; i < MAX_PROCESSES; i++) {
                pcb_t *zombie = &process_table[i];
                if (zombie->state == PROCESS_ZOMBIE &&
                    zombie->pid != current_pid &&
                    !process_has_waiter(zombie->pid)) {
                        process_free_resources(zombie->pid);
                        zombie->state = PROCESS_DEAD;
                }
        }
}

/* True if a live process is waiting on pid (so only the waiter should reap
 * it). ZOMBIE/DEAD processes are ignored: a waiter that itself died (e.g. was
 * killed mid-wait) leaves a stale waiting_for that must not pin the zombie. */
int process_has_waiter(pid_t pid) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                process_state_t st = process_table[i].state;
                if (st != PROCESS_DEAD && st != PROCESS_ZOMBIE &&
                    process_table[i].waiting_for == pid)
                        return 1;
        }
        return 0;
}

static pcb_t *find_free_slot(void) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                if (process_table[i].state == PROCESS_DEAD)
                        return &process_table[i];
        }
        return NULL;
}

static void setup_user_stack(uint8_t *ustack, uint64_t exit_handler,
                             uint64_t *out_rsp) {
        uint64_t top = (uint64_t)ustack + USER_STACK_SIZE;
        top -= sizeof(uint64_t);
        *(uint64_t *)top = exit_handler;
        *out_rsp         = top;
}

static void setup_kernel_stack(uint8_t *kstack, uint64_t entry, uint64_t argc,
                               char **argv, uint64_t user_rsp,
                               uint64_t *out_rsp) {
        uint64_t top = (uint64_t)kstack + KERNEL_STACK_SIZE;
        top -= sizeof(context_frame_t);
        context_frame_t *frame = (context_frame_t *)top;
        memset(frame, 0, sizeof(context_frame_t));

        frame->rip    = entry;
        frame->cs     = USER_CS;
        frame->rflags = RFLAGS_IF;
        frame->rsp    = user_rsp;
        frame->ss     = USER_SS;
        frame->rdi    = argc;
        frame->rsi    = (uint64_t)argv;

        *out_rsp = top;
}

void process_init(void) {
        /* Capture a clean FPU state once; new processes start from this copy. */
        fpu_init_area(fpu_template);

        for (int i = 0; i < MAX_PROCESSES; i++) {
                process_table[i].state             = PROCESS_DEAD;
                process_table[i].pid               = NO_PID;
                process_table[i].kernel_stack_base = NULL;
                process_table[i].user_stack_base   = NULL;
                process_table[i].fpu_area          = NULL;
        }

        pcb_t *shell             = &process_table[SHELL_INDEX];
        shell->pid               = next_pid_to_assign++;
        shell->state             = PROCESS_RUNNING;
        shell->priority          = DEFAULT_PRIORITY;
        shell->sched_credits     = 0;
        shell->sleep_until_ms    = 0;
        shell->foreground        = 1;
        shell->parent_pid        = NO_PID;
        shell->waiting_for       = NO_PID;
        shell->stdin_pipe        = NO_PIPE;
        shell->stdout_pipe       = NO_PIPE;
        shell->blocked_on_pipe   = NO_PIPE;
        shell->blocked_on_keyboard = 0;
        shell->argv_copy         = NULL;
        shell->kernel_stack_base = NULL;
        shell->user_stack_base   = NULL;
        shell->fpu_area          = alloc_fpu_area();
        shell->user_allocs.next  = shell->user_allocs.prev = &shell->user_allocs;
        strncpy(shell->name, "sh", PROCESS_NAME_LEN);

        current_pid = shell->pid;
}

pcb_t *process_get_current(void) {
        return process_get(current_pid);
}

pcb_t *process_get(pid_t pid) {
        if (pid < 0)
                return NULL;
        for (int i = 0; i < MAX_PROCESSES; i++) {
                if (process_table[i].pid == pid &&
                    process_table[i].state != PROCESS_DEAD)
                        return &process_table[i];
        }
        return NULL;
}

pcb_t *process_get_by_index(int idx) {
        if (idx < 0 || idx >= MAX_PROCESSES)
                return NULL;
        if (process_table[idx].state == PROCESS_DEAD)
                return NULL;
        return &process_table[idx];
}

pid_t process_getpid(void) {
        return current_pid;
}

pid_t process_create(uint64_t entry, uint64_t argc, char **argv,
                     const char *name, int foreground, uint64_t exit_handler,
                     int stdin_pipe, int stdout_pipe) {
        /* Build the whole PCB atomically: the process must not become
         * schedulable (state READY) until its stack frame and rsp are set up,
         * otherwise a timer tick could pick it and iretq into a garbage frame. */
        uint64_t flags = irq_save();

        pcb_t *pcb = find_free_slot();
        if (pcb == NULL) {
                irq_restore(flags);
                return NO_PID;
        }

        uint8_t *kstack = (uint8_t *)mem_alloc(KERNEL_STACK_SIZE);
        uint8_t *ustack = (uint8_t *)mem_alloc(USER_STACK_SIZE);
        if (kstack == NULL || ustack == NULL) {
                if (kstack)
                        mem_free(kstack);
                if (ustack)
                        mem_free(ustack);
                irq_restore(flags);
                return NO_PID;
        }

        char **argv_copy = clone_argv(argc, argv);
        if (argc > 0 && argv_copy == NULL) {
                mem_free(kstack);
                mem_free(ustack);
                irq_restore(flags);
                return NO_PID;
        }

        uint8_t *fpu_area = alloc_fpu_area();
        if (fpu_area == NULL) {
                mem_free(kstack);
                mem_free(ustack);
                if (argv_copy)
                        mem_free(argv_copy);
                irq_restore(flags);
                return NO_PID;
        }

        pcb->pid                  = next_pid_to_assign++;
        pcb->state                = PROCESS_READY;
        pcb->blocked_by_semaphore = 0;
        pcb->priority             = DEFAULT_PRIORITY;
        pcb->sched_credits        = 0; /* refilled on the next scheduling round */
        pcb->sleep_until_ms       = 0;
        pcb->foreground           = foreground;
        pcb->parent_pid           = current_pid;
        pcb->waiting_for          = NO_PID;
        pcb->exit_code            = 0;
        pcb->stdin_pipe           = stdin_pipe;
        pcb->stdout_pipe          = stdout_pipe;
        pcb->blocked_on_pipe      = NO_PIPE;
        pcb->blocked_on_keyboard  = 0;
        pcb->argv_copy            = argv_copy;
        pcb->fpu_area             = fpu_area;
        pcb->kernel_stack_base    = kstack;
        pcb->user_stack_base      = ustack;
        pcb->user_allocs.next     = pcb->user_allocs.prev = &pcb->user_allocs;
        strncpy(pcb->name, name ? name : "unknown", PROCESS_NAME_LEN);

        if (stdout_pipe != NO_PIPE)
                pipe_mark_writer(stdout_pipe);

        uint64_t user_rsp;
        setup_user_stack(ustack, exit_handler, &user_rsp);
        setup_kernel_stack(kstack, entry, argc, argv_copy, user_rsp, &pcb->rsp);

        irq_restore(flags);
        return pcb->pid;
}

void process_exit(int code) {
        pcb_t *pcb = process_get_current();
        pipe_cleanup_process(pcb->stdin_pipe, pcb->stdout_pipe);
        sem_remove_from_queues(pcb->pid);
        pcb->stdin_pipe      = NO_PIPE;
        pcb->stdout_pipe     = NO_PIPE;
        pcb->blocked_on_pipe = NO_PIPE;

        // Avoiding race conditions and leaving orphan processes
        uint64_t flags = irq_save();
        pcb->state           = PROCESS_ZOMBIE;
        pcb->exit_code       = code;
        wake_waiters(pcb->pid);
        reparent_and_reap_orphans(pcb->pid);
        irq_restore(flags);

        scheduler_yield();
        while (1)
                _hlt();
}

int process_kill(pid_t pid) {
        if (pid == SHELL_PID)
                return SYS_ERR;
        pcb_t *pcb = process_get(pid);
        if (pcb == NULL || pcb->state == PROCESS_ZOMBIE)
                return SYS_ERR;

        pipe_cleanup_process(pcb->stdin_pipe, pcb->stdout_pipe);
        sem_remove_from_queues(pid);
        pcb->stdin_pipe      = NO_PIPE;
        pcb->stdout_pipe     = NO_PIPE;
        pcb->blocked_on_pipe = NO_PIPE;

        uint64_t flags = irq_save();
        pcb->state           = PROCESS_ZOMBIE;
        pcb->exit_code       = KILLED_EXIT_CODE;
        wake_waiters(pid);
        reparent_and_reap_orphans(pid);
        irq_restore(flags);

        if (pid == current_pid) {
                scheduler_yield();
        }

        return SYS_OK;
}

// - process_block
// - process_unblock
// - process_nice
// - block_by_semaphore
// - unblock_by_semaphore

// Race conditions are covered because caller disables interrupts
int block_by_semaphore(pid_t pid) {
        pcb_t *pcb = process_get(pid);
        if (pcb == NULL ||
            (pcb->state != PROCESS_READY && pcb->state != PROCESS_RUNNING))
                return SYS_ERR;

        pcb->blocked_by_semaphore = 1;
        pcb->state                = PROCESS_BLOCKED;

        return SYS_OK;
}

int process_block(pid_t pid) {
        pcb_t *pcb = process_get(pid);
        uint64_t flags = irq_save();
        if (pcb == NULL ||
            (pcb->state != PROCESS_READY && pcb->state != PROCESS_RUNNING))
                return SYS_ERR;

        pcb->state = PROCESS_BLOCKED;
        irq_restore(flags);

        if (pid == current_pid)
                scheduler_yield();

        return SYS_OK;
}

int unblock_by_semaphore(pid_t pid) {
        pcb_t *pcb = process_get(pid);
        if (pcb == NULL || pcb->state != PROCESS_BLOCKED ||
            !pcb->blocked_by_semaphore)
                return SYS_ERR;

        pcb->blocked_by_semaphore = 0;
        pcb->state                = PROCESS_READY;
        return SYS_OK;
}

int process_unblock(pid_t pid) {
        pcb_t *pcb = process_get(pid);
        if (pcb == NULL || pcb->state != PROCESS_BLOCKED)
                return SYS_ERR;

        pcb->state = PROCESS_READY;
        return SYS_OK;
}

int process_nice(pid_t pid, uint64_t new_priority) {
        pcb_t *pcb = process_get(pid);
        if (pcb == NULL || pcb->state == PROCESS_ZOMBIE)
                return SYS_ERR;
        if (new_priority > MAX_PRIORITY)
                new_priority = MAX_PRIORITY;
        if (new_priority < 1)
                new_priority = 1;

        pcb->priority = new_priority;
        return SYS_OK;
}

static pcb_t *find_by_pid_any_state(pid_t pid) {
        if (pid < 0)
                return NULL;
        for (int i = 0; i < MAX_PROCESSES; i++) {
                if (process_table[i].pid == pid)
                        return &process_table[i];
        }
        return NULL;
}

static int reap_zombie(pcb_t *target, pid_t pid) {
        int code = target->exit_code;
        process_free_resources(pid);
        target->state = PROCESS_DEAD;
        return code;
}

int process_wait(pid_t pid) {
        pcb_t *target = find_by_pid_any_state(pid);
        if (target == NULL || target->state == PROCESS_DEAD)
                return SYS_ERR;

        pcb_t *current = process_get_current();

        for (;;) {
                /* Test-and-block atomically with interrupts off: syscalls run
                 * with IF on, so otherwise a tick could exit the child (and run
                 * wake_waiters) after the test but before we block, losing the
                 * wakeup. Same discipline as sem_wait. */
                uint64_t flags = irq_save();
                if (target->pid != pid || target->state == PROCESS_ZOMBIE ||
                    target->state == PROCESS_DEAD) {
                        irq_restore(flags);
                        break;
                }
                current->waiting_for = pid;
                current->state       = PROCESS_BLOCKED;
                irq_restore(flags);

                scheduler_yield();
                while (current->state == PROCESS_BLOCKED)
                        _hlt();
        }

        /* Done waiting: clear our claim so the scheduler may reap future
         * orphan zombies, and so a stale waiting_for never lingers. */
        current->waiting_for = NO_PID;

        if (target->pid != pid)
                return SYS_ERR; /* slot reused, child was already reaped */
        if (target->state == PROCESS_ZOMBIE)
                return reap_zombie(target, pid);
        return target->exit_code; /* scheduler/kill already reaped it */
}

int process_list(uint64_t *pids, int max) {
        int count = 0;
        for (int i = 0; i < MAX_PROCESSES && count < max; i++) {
                if (process_table[i].state != PROCESS_DEAD)
                        pids[count++] = process_table[i].pid;
        }
        return count;
}

int process_snapshot(process_info_t *info, int max) {
        int count = 0;
        for (int i = 0; i < MAX_PROCESSES && count < max; i++) {
                pcb_t *pcb = &process_table[i];
                if (pcb->state == PROCESS_DEAD)
                        continue;
                info[count].pid        = pcb->pid;
                info[count].priority   = pcb->priority;
                info[count].rsp        = pcb->rsp;
                info[count].stack_base = (uint64_t)pcb->user_stack_base;
                info[count].state      = pcb->state;
                info[count].foreground = pcb->foreground;
                strncpy(info[count].name, pcb->name, PROCESS_INFO_NAME_LEN);
                count++;
        }
        return count;
}

int process_kill_foreground(void) {
        int killed = 0;
        for (int i = 0; i < MAX_PROCESSES; i++) {
                pcb_t *pcb = &process_table[i];
                if (pcb->state == PROCESS_DEAD ||
                    pcb->state == PROCESS_ZOMBIE)
                        continue;
                if (pcb->foreground && pcb->pid != SHELL_PID) {
                        process_kill(pcb->pid);
                        killed++;
                }
        }
        return killed;
}

void process_wake_keyboard_readers(void) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                pcb_t *pcb = &process_table[i];
                if (pcb->state == PROCESS_BLOCKED &&
                    pcb->blocked_on_keyboard) {
                        pcb->blocked_on_keyboard = 0;
                        pcb->state               = PROCESS_READY;
                }
        }
}

void process_sleep_ms(uint64_t ms) {
        pcb_t *self = process_get_current();
        if (self == NULL || ms == 0)
                return;

        /* Set the deadline and block atomically (same discipline as
         * process_wait) a timer tick must not wake us between the two. */
        uint64_t flags = irq_save();
        self->sleep_until_ms = get_time_ms() + ms;
        self->state          = PROCESS_BLOCKED;
        irq_restore(flags);

        while (self->state == PROCESS_BLOCKED)
                _hlt();
        self->sleep_until_ms = 0;
}

void process_wake_sleepers(uint64_t now_ms) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                pcb_t *pcb = &process_table[i];
                if (pcb->state == PROCESS_BLOCKED && pcb->sleep_until_ms != 0 &&
                    now_ms >= pcb->sleep_until_ms) {
                        pcb->sleep_until_ms = 0;
                        pcb->state          = PROCESS_READY;
                }
        }
}
