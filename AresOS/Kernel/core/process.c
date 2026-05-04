#include <interrupts.h>
#include <lib_common.h>
#include <multi_region_heap.h>
#include <process.h>

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

void process_exit(int code) {
        pcb_t *pcb     = process_get_current();
        pcb->state     = PROCESS_DEAD;
        pcb->exit_code = code;
        wake_waiters(pcb->pid);
        halt_until_switched();
}

