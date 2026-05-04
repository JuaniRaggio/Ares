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
        return (process_entry_t)0;
}

int64_t my_create_process(char *name, uint64_t argc, char *argv[]) {
        process_entry_t func = lookup_function(name);
        if (func == (process_entry_t)0)
                return -1;

        create_process_info_t info;
        info.entry        = (uint64_t)func;
        info.argc         = argc;
        info.argv         = argv;
        info.name         = name;
        info.foreground   = 0;
        info.exit_handler = (uint64_t)_process_exit_stub;

        return syscall_create_process(&info);
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
        while (1)
                syscall_yield();
}
