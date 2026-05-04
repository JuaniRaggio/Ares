#include <interrupts.h>
#include <lib_common.h>
#include <multi_region_heap.h>
#include <process.h>

#define USER_CS          0x1B
#define USER_SS          0x23
#define RFLAGS_IF        0x202
#define NO_PID           (-1)
#define SHELL_PID        0
#define FIRST_USER_PID   1
#define KILLED_EXIT_CODE (-1)

static pcb_t process_table[MAX_PROCESSES];
static pid_t current_pid;

extern void scheduler_yield(void);

void process_set_current_pid(pid_t pid) {
        current_pid = pid;
}

void process_free_resources(pid_t pid) {
        if (pid < 0 || pid >= MAX_PROCESSES)
                return;
        pcb_t *pcb = &process_table[pid];
        if (pcb->kernel_stack_base != (void *)0) {
                mem_free(pcb->kernel_stack_base);
                pcb->kernel_stack_base = (void *)0;
        }
        if (pcb->user_stack_base != (void *)0) {
                mem_free(pcb->user_stack_base);
                pcb->user_stack_base = (void *)0;
        }
}

static void wake_waiters(pid_t dead_pid) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                if (process_table[i].state == PROCESS_BLOCKED &&
                    process_table[i].waiting_for == dead_pid) {
                        process_table[i].state       = PROCESS_READY;
                        process_table[i].waiting_for = NO_PID;
                }
        }
}

static pcb_t *find_free_slot(void) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                if (process_table[i].state == PROCESS_DEAD)
                        return &process_table[i];
        }
        return (void *)0;
}

static void setup_user_stack(uint8_t *ustack, uint64_t exit_handler,
                             uint64_t *out_rsp) {
        uint64_t top = (uint64_t)ustack + PROCESS_STACK_SIZE;
        top -= sizeof(uint64_t);
        *(uint64_t *)top = exit_handler;
        *out_rsp         = top;
}

static void setup_kernel_stack(uint8_t *kstack, uint64_t entry, uint64_t argc,
                               char **argv, uint64_t user_rsp,
                               uint64_t *out_rsp) {
        uint64_t top = (uint64_t)kstack + PROCESS_STACK_SIZE;
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
        for (int i = 0; i < MAX_PROCESSES; i++) {
                process_table[i].state             = PROCESS_DEAD;
                process_table[i].pid               = i;
                process_table[i].kernel_stack_base = (void *)0;
                process_table[i].user_stack_base   = (void *)0;
        }

        pcb_t *shell               = &process_table[SHELL_PID];
        shell->state               = PROCESS_RUNNING;
        shell->priority            = DEFAULT_PRIORITY;
        shell->foreground          = 1;
        shell->parent_pid          = NO_PID;
        shell->waiting_for         = NO_PID;
        shell->kernel_stack_base   = (void *)0; /* Static boot stacks */
        shell->user_stack_base     = (void *)0;
        strncpy(shell->name, "shell", PROCESS_NAME_LEN);

        current_pid = SHELL_PID;
}

pcb_t *process_get_current(void) {
        return &process_table[current_pid];
}

pcb_t *process_get(pid_t pid) {
        if (pid < 0 || pid >= MAX_PROCESSES)
                return (void *)0;
        if (process_table[pid].state == PROCESS_DEAD)
                return (void *)0;
        return &process_table[pid];
}

pid_t process_getpid(void) {
        return current_pid;
}

pid_t process_create(uint64_t entry, uint64_t argc, char **argv,
                     const char *name, int foreground, uint64_t exit_handler) {
        pcb_t *pcb = find_free_slot();
        if (pcb == (void *)0)
                return NO_PID;

        uint8_t *kstack = (uint8_t *)mem_alloc(PROCESS_STACK_SIZE);
        uint8_t *ustack = (uint8_t *)mem_alloc(PROCESS_STACK_SIZE);
        if (kstack == (void *)0 || ustack == (void *)0) {
                if (kstack)
                        mem_free(kstack);
                if (ustack)
                        mem_free(ustack);
                return NO_PID;
        }

        pcb->state             = PROCESS_READY;
        pcb->priority          = DEFAULT_PRIORITY;
        pcb->foreground        = foreground;
        pcb->parent_pid        = current_pid;
        pcb->waiting_for       = NO_PID;
        pcb->exit_code         = 0;
        pcb->kernel_stack_base = kstack;
        pcb->user_stack_base   = ustack;
        strncpy(pcb->name, name ? name : "unknown", PROCESS_NAME_LEN);

        uint64_t user_rsp;
        setup_user_stack(ustack, exit_handler, &user_rsp);
        setup_kernel_stack(kstack, entry, argc, argv, user_rsp, &pcb->rsp);

        return pcb->pid;
}

void process_exit(int code) {
        pcb_t *pcb     = process_get_current();
        pcb->state     = PROCESS_ZOMBIE;
        pcb->exit_code = code;
        wake_waiters(pcb->pid);
        scheduler_yield();
        while (1)
                _hlt();
}

int process_kill(pid_t pid) {
        if (pid < 0 || pid >= MAX_PROCESSES)
                return -1;
        pcb_t *pcb = &process_table[pid];
        if (pcb->state == PROCESS_DEAD || pcb->state == PROCESS_ZOMBIE)
                return -1;

        pcb->state     = PROCESS_ZOMBIE;
        pcb->exit_code = KILLED_EXIT_CODE;
        wake_waiters(pid);

        if (pid == current_pid) {
                scheduler_yield();
                /* Should not return if it's the current process */
        }

        return 0;
}

int process_block(pid_t pid) {
        if (pid < 0 || pid >= MAX_PROCESSES)
                return -1;
        pcb_t *pcb = &process_table[pid];
        if (pcb->state != PROCESS_READY && pcb->state != PROCESS_RUNNING)
                return -1;

        pcb->state = PROCESS_BLOCKED;

        if (pid == current_pid)
                scheduler_yield();

        return 0;
}

int process_unblock(pid_t pid) {
        if (pid < 0 || pid >= MAX_PROCESSES)
                return -1;
        pcb_t *pcb = &process_table[pid];
        if (pcb->state != PROCESS_BLOCKED)
                return -1;

        pcb->state = PROCESS_READY;
        return 0;
}

int process_nice(pid_t pid, uint64_t new_priority) {
        if (pid < 0 || pid >= MAX_PROCESSES)
                return -1;
        pcb_t *pcb = &process_table[pid];
        if (pcb->state == PROCESS_DEAD || pcb->state == PROCESS_ZOMBIE)
                return -1;
        if (new_priority > MAX_PRIORITY)
                new_priority = MAX_PRIORITY;
        if (new_priority < 1)
                new_priority = 1;

        pcb->priority = new_priority;
        return 0;
}

static int reap_zombie(pcb_t *target, pid_t pid) {
        int code = target->exit_code;
        process_free_resources(pid);
        target->state = PROCESS_DEAD;
        return code;
}

int process_wait(pid_t pid) {
        if (pid < 0 || pid >= MAX_PROCESSES)
                return -1;
        pcb_t *target = &process_table[pid];

        if (target->state == PROCESS_DEAD)
                return -1;
        if (target->state == PROCESS_ZOMBIE)
                return reap_zombie(target, pid);

        pcb_t *current       = process_get_current();
        current->state       = PROCESS_BLOCKED;
        current->waiting_for = pid;

        scheduler_yield();

        return reap_zombie(target, pid);
}

int process_list(uint64_t *pids, int max) {
        int count = 0;
        for (int i = 0; i < MAX_PROCESSES && count < max; i++) {
                if (process_table[i].state != PROCESS_DEAD)
                        pids[count++] = process_table[i].pid;
        }
        return count;
}

