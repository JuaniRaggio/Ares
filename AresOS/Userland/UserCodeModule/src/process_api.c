#include <stddef.h>
#include <process_api.h>
#include <process_types.h>
#include <syscalls.h>
#include <lib_common.h>

#define MAX_REGISTERED_FUNCS 64

extern void _process_exit_stub(void);

typedef struct {
        const char *name;
        process_entry_t func;
} func_entry_t;

static func_entry_t registry[MAX_REGISTERED_FUNCS];
static int registry_count;

void process_register(const char *name, process_entry_t func) {
        if (registry_count >= MAX_REGISTERED_FUNCS)
                return;
        registry[registry_count].name = name;
        registry[registry_count].func = func;
        registry_count++;
}

static process_entry_t lookup_function(const char *name) {
        for (int i = 0; i < registry_count; i++) {
                if (strcmp(registry[i].name, name) == 0)
                        return registry[i].func;
        }
        return NULL;
}

int process_is_registered(const char *name) {
        return lookup_function(name) != NULL;
}

int64_t my_spawn(char *name, uint64_t argc, char *argv[], int foreground,
                 int stdin_pipe, int stdout_pipe) {
        process_entry_t func = lookup_function(name);
        if (func == NULL)
                return NO_PID;

        create_process_info_t info;
        info.entry        = (uint64_t)func;
        info.argc         = argc;
        info.argv         = argv;
        info.name         = name;
        info.foreground   = foreground;
        info.exit_handler = (uint64_t)_process_exit_stub;
        info.stdin_pipe   = stdin_pipe;
        info.stdout_pipe  = stdout_pipe;

        return syscall_create_process(&info);
}

int64_t my_create_process(char *name, uint64_t argc, char *argv[]) {
        return my_spawn(name, argc, argv, 0, NO_PIPE, NO_PIPE);
}

int64_t my_create_process_piped(char *name, uint64_t argc, char *argv[],
                                int stdin_pipe, int stdout_pipe) {
        return my_spawn(name, argc, argv, 0, stdin_pipe, stdout_pipe);
}

int my_pipe_open(const char *name) {
        return syscall_pipe_open(name);
}

int my_pipe_close(int pipe_id) {
        return syscall_pipe_close(pipe_id);
}

int64_t my_getpid(void) {
        return (int64_t)syscall_getpid();
}

int64_t my_nice(uint64_t pid, uint64_t newPrio) {
        return syscall_nice(pid, newPrio);
}

int64_t my_kill(uint64_t pid) {
        return syscall_kill(pid);
}

int64_t my_block(uint64_t pid) {
        return syscall_block(pid);
}

int64_t my_unblock(uint64_t pid) {
        return syscall_unblock(pid);
}

int64_t my_yield(void) {
        return (int64_t)syscall_yield();
}

int64_t my_wait(int64_t pid) {
        return syscall_waitpid((uint64_t)pid);
}

int64_t my_list_processes(uint64_t *pids, int max) {
        return syscall_list_processes(pids, (uint64_t)max);
}

void idle_process(void) {
        /* Sleep until the next interrupt instead of busy-waiting: idle runs in
         * ring 3 and cannot hlt directly, so it asks the kernel to. */
        while (1)
                syscall_halt();
}
