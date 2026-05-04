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

