#pragma once

/**
 * @file process.h
 * @brief Process management subsystem.
 *
 * Provides PCB structures, process lifecycle operations (create, exit, kill),
 * state transitions (block, unblock), priority management, and waitpid.
 */

#include <stdint.h>

#define MAX_PROCESSES 32
#define PROCESS_NAME_LEN 32
#define PROCESS_STACK_SIZE (4096 * 4)
#define DEFAULT_PRIORITY 1
#define MAX_PRIORITY 4

typedef int64_t pid_t;
typedef uint64_t (*process_func_t)(uint64_t argc, char *argv[]);

/** @brief Process states. */
typedef enum {
        PROCESS_READY   = 0,
        PROCESS_RUNNING = 1,
        PROCESS_BLOCKED = 2,
        PROCESS_DEAD    = 3,
        PROCESS_ZOMBIE  = 4
} process_state_t;

/**
 * @brief Internal use by scheduler to update the current PID.
 * @param pid The new current PID.
 */
void process_set_current_pid(pid_t pid);

/**
 * @brief Internal use by scheduler to free resources of a zombie process.
 * @param pid PID of the zombie process.
 */
void process_free_resources(pid_t pid);

/** @brief Process Control Block. */
typedef struct {
        pid_t pid;
        char name[PROCESS_NAME_LEN];
        uint64_t rsp;
        uint8_t *kernel_stack_base;
        uint8_t *user_stack_base;
        process_state_t state;
        uint64_t priority;
        int foreground;
        pid_t parent_pid;
        int exit_code;
        pid_t waiting_for;
} pcb_t;

/**
 * @brief Stack frame layout matching pushState + iretq.
 *
 * Built on the kernel stack for new processes. The scheduler's popState + iretq
 * restores this frame to start the process in ring 3.
 */
typedef struct {
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
        uint64_t rip, cs, rflags, rsp, ss;
} context_frame_t;

typedef char _assert_frame_size[(sizeof(context_frame_t) == 20 * 8) ? 1 : -1];

/** @brief Initialize the process subsystem and create the shell (pid 0). */
void process_init(void);

/**
 * @brief Create a new process.
 * @param entry Function entry point address.
 * @param argc Argument count passed to the entry function.
 * @param argv Argument vector passed to the entry function.
 * @param name Human-readable process name.
 * @param foreground Whether this is a foreground process.
 * @param exit_handler Userland address to jump to when the function returns.
 * @return PID of the new process, or -1 on failure.
 */
pid_t process_create(uint64_t entry, uint64_t argc, char **argv,
                     const char *name, int foreground, uint64_t exit_handler);

/**
 * @brief Terminate the current process with the given exit code. Never returns.
 * @param code Exit code.
 */
void process_exit(int code);

/**
 * @brief Return the PID of the calling process.
 * @return PID of the caller.
 */
pid_t process_getpid(void);

/**
 * @brief Kill a process by PID.
 * @param pid PID of the process to kill.
 * @return 0 on success, -1 on error.
 */
int process_kill(pid_t pid);

/**
 * @brief Block a process. If pid is the caller, blocks until unblocked.
 * @param pid PID of the process to block.
 * @return 0 on success, -1 on error.
 */
int process_block(pid_t pid);

/**
 * @brief Unblock a blocked process.
 * @param pid PID of the process to unblock.
 * @return 0 on success, -1 if not blocked.
 */
int process_unblock(pid_t pid);

/**
 * @brief Change a process's scheduling priority.
 * @param pid PID of the target process.
 * @param new_priority New priority value (0 to MAX_PRIORITY).
 * @return 0 on success, -1 on error.
 */
int process_nice(pid_t pid, uint64_t new_priority);

/**
 * @brief Block the caller until the target process exits.
 * @param pid PID to wait for.
 * @return Exit code of the waited process, or -1 on error.
 */
int process_wait(pid_t pid);

/**
 * @brief Get the PCB of the currently running process.
 * @return Pointer to the current process's PCB.
 */
pcb_t *process_get_current(void);

/**
 * @brief Get the PCB for a given PID.
 * @param pid PID to look up.
 * @return Pointer to the PCB, or NULL if dead or invalid.
 */
pcb_t *process_get(pid_t pid);

/**
 * @brief Get the PCB for a given index in the process table.
 * @param idx Index to look up.
 * @return Pointer to the PCB, or NULL if dead or invalid index.
 */
pcb_t *process_get_by_index(int idx);

/**
 * @brief Fill pids array with active process PIDs.
 * @param pids Array to fill with PIDs.
 * @param max Maximum entries to fill.
 * @return Number of PIDs written.
 */
int process_list(uint64_t *pids, int max);
