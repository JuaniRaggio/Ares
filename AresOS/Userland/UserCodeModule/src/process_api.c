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
