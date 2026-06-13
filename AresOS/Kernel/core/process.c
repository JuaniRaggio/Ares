#include <stddef.h>
#include <interrupts.h>
#include <lib_common.h>
#include <multi_region_heap.h>
#include <pipe.h>
#include <process.h>
#include <semaphores.h>
#include <status_codes.h>

/* GDT layout puts user data (0x18) before user code (0x20) as SYSRET
 * requires; both selectors carry RPL=3 */
#define USER_CS 0x23
#define USER_SS 0x1B
#define RFLAGS_IF 0x202
#define SHELL_INDEX 0

static pcb_t process_table[MAX_PROCESSES];
static pid_t current_pid;
static pid_t next_pid_to_assign = 0;

extern void scheduler_yield(void);

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
        return NULL;
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
                process_table[i].pid               = NO_PID;
                process_table[i].kernel_stack_base = NULL;
                process_table[i].user_stack_base   = NULL;
        }

        pcb_t *shell             = &process_table[SHELL_INDEX];
        shell->pid               = next_pid_to_assign++;
        shell->state             = PROCESS_RUNNING;
        shell->priority          = DEFAULT_PRIORITY;
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
        strncpy(shell->name, "shell", PROCESS_NAME_LEN);

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
        pcb_t *pcb = find_free_slot();
        if (pcb == NULL)
                return NO_PID;

        uint8_t *kstack = (uint8_t *)mem_alloc(PROCESS_STACK_SIZE);
        uint8_t *ustack = (uint8_t *)mem_alloc(PROCESS_STACK_SIZE);
        if (kstack == NULL || ustack == NULL) {
                if (kstack)
                        mem_free(kstack);
                if (ustack)
                        mem_free(ustack);
                return NO_PID;
        }

        char **argv_copy = clone_argv(argc, argv);
        if (argc > 0 && argv_copy == NULL) {
                mem_free(kstack);
                mem_free(ustack);
                return NO_PID;
        }

        pcb->pid                  = next_pid_to_assign++;
        pcb->state                = PROCESS_READY;
        pcb->blocked_by_semaphore = 0;
        pcb->priority             = DEFAULT_PRIORITY;
        pcb->foreground           = foreground;
        pcb->parent_pid           = current_pid;
        pcb->waiting_for          = NO_PID;
        pcb->exit_code            = 0;
        pcb->stdin_pipe           = stdin_pipe;
        pcb->stdout_pipe          = stdout_pipe;
        pcb->blocked_on_pipe      = NO_PIPE;
        pcb->blocked_on_keyboard  = 0;
        pcb->argv_copy            = argv_copy;
        pcb->kernel_stack_base    = kstack;
        pcb->user_stack_base      = ustack;
        strncpy(pcb->name, name ? name : "unknown", PROCESS_NAME_LEN);

        if (stdout_pipe != NO_PIPE)
                pipe_mark_writer(stdout_pipe);

        uint64_t user_rsp;
        setup_user_stack(ustack, exit_handler, &user_rsp);
        setup_kernel_stack(kstack, entry, argc, argv_copy, user_rsp, &pcb->rsp);

        return pcb->pid;
}

void process_exit(int code) {
        pcb_t *pcb = process_get_current();
        pipe_cleanup_process(pcb->stdin_pipe, pcb->stdout_pipe);
        sem_remove_from_queues(pcb->pid);
        pcb->stdin_pipe      = NO_PIPE;
        pcb->stdout_pipe     = NO_PIPE;
        pcb->blocked_on_pipe = NO_PIPE;
        pcb->state           = PROCESS_ZOMBIE;
        pcb->exit_code       = code;
        wake_waiters(pcb->pid);
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
        pcb->state           = PROCESS_ZOMBIE;
        pcb->exit_code       = KILLED_EXIT_CODE;
        wake_waiters(pid);

        if (pid == current_pid) {
                scheduler_yield();
        }

        return SYS_OK;
}

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
        if (pcb == NULL ||
            (pcb->state != PROCESS_READY && pcb->state != PROCESS_RUNNING))
                return SYS_ERR;

        pcb->state = PROCESS_BLOCKED;

        if (pid == current_pid)
                scheduler_yield();

        return SYS_OK;
}

int unblock_by_semaphore(pid_t pid) {
        pcb_t *pcb = process_get(pid);
        if (pcb == NULL || pcb->state != PROCESS_BLOCKED ||
            !pcb->blocked_by_semaphore)
                return -1;

        pcb->blocked_by_semaphore = 0;
        pcb->state                = PROCESS_READY;
        return 0;
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

        /* scheduler_yield() only drops the quantum: the actual switch
         * happens on the next timer tick, so loop until the child really
         * finished instead of checking once and returning early. */
        while (target->pid == pid && target->state != PROCESS_ZOMBIE &&
               target->state != PROCESS_DEAD) {
                /* waiting_for must be set BEFORE blocking: if a timer tick
                 * lands in between, wake_waiters would not match us and the
                 * wakeup would be lost forever. */
                current->waiting_for = pid;
                current->state       = PROCESS_BLOCKED;
                scheduler_yield();
                _hlt();
        }

        if (target->pid != pid)
                return SYS_ERR; /* slot reused, child was already reaped */
        if (target->state == PROCESS_ZOMBIE)
                return reap_zombie(target, pid);
        return target->exit_code;
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
