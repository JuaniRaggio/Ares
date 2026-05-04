#include <interrupts.h>
#include <lib_common.h>
#include <multi_region_heap.h>
#include <process.h>

#define USER_CS       0x1B
#define USER_SS       0x23
#define RFLAGS_IF     0x202
#define NO_PID        (-1)
#define SHELL_PID     0
#define FIRST_USER_PID 1
#define KILLED_EXIT_CODE (-1)

static pcb_t process_table[MAX_PROCESSES];
static pid_t current_pid;
static pid_t next_pid;

extern uint8_t kernel_stack[];
extern uint8_t kernel_stack_top[];
extern uint8_t user_stack[];
extern uint8_t user_stack_top[];

static void halt_until_switched(void) {
        while (1)
                _hlt();
}

static void halt_while_blocked(pid_t pid) {
        while (process_table[pid].state == PROCESS_BLOCKED)
                _hlt();
}

static void wake_waiters(pid_t dead_pid) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                if (process_table[i].state == PROCESS_BLOCKED &&
                    process_table[i].waiting_for == dead_pid) {
                        process_table[i].state = PROCESS_READY;
                        process_table[i].waiting_for = NO_PID;
                }
        }
}

static pcb_t *find_free_slot(void) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
                int idx = (next_pid + i) % MAX_PROCESSES;
                if (process_table[idx].state == PROCESS_DEAD)
                        return &process_table[idx];
        }
        return (void *)0;
}

static void setup_user_stack(uint8_t *ustack, uint64_t exit_handler,
}

static void setup_kernel_stack(uint8_t *kstack, uint64_t entry, uint64_t argc,
}

void process_init(void) {
        for (int i = 0; i < MAX_PROCESSES; i++)
                process_table[i].state = PROCESS_DEAD;

        pcb_t *shell               = &process_table[SHELL_PID];
        shell->pid                 = SHELL_PID;
        shell->state               = PROCESS_RUNNING;
        shell->priority            = DEFAULT_PRIORITY;
        shell->foreground          = 1;
        shell->parent_pid          = NO_PID;
        shell->waiting_for         = NO_PID;
        shell->kernel_stack_base   = kernel_stack;
        shell->user_stack_base     = user_stack;
        strncpy(shell->name, "shell", PROCESS_NAME_LEN);

        current_pid = SHELL_PID;
        next_pid    = FIRST_USER_PID;
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
}

void process_exit(int code) {
        pcb_t *pcb     = process_get_current();
        pcb->state     = PROCESS_DEAD;
        pcb->exit_code = code;
        wake_waiters(pcb->pid);
        halt_until_switched();
}

int process_kill(pid_t pid) {
}

int process_block(pid_t pid) {
}

int process_unblock(pid_t pid) {
}

int process_nice(pid_t pid, uint64_t new_priority) {
}

int process_wait(pid_t pid) {
}

int process_list(uint64_t *pids, int max) {
        int count = 0;
        for (int i = 0; i < MAX_PROCESSES && count < max; i++) {
                if (process_table[i].state != PROCESS_DEAD)
                        pids[count++] = process_table[i].pid;
        }
        return count;
}
