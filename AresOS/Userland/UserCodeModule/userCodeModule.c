/* userCodeModule.c */

#include <process_api.h>
#include <shell.h>

extern uint64_t test_mm(uint64_t argc, char *argv[]);
extern uint64_t test_prio(uint64_t argc, char *argv[]);
extern uint64_t test_processes(uint64_t argc, char *argv[]);
extern uint64_t test_sync(uint64_t argc, char *argv[]);

extern void endless_loop(void);
extern void endless_loop_print(uint64_t wait);

int main() {
        process_register("idle", (process_entry_t)idle_process);

        process_register("test_mm", (process_entry_t)test_mm);
        process_register("test_prio", (process_entry_t)test_prio);
        process_register("test_processes", (process_entry_t)test_processes);
        process_register("test_sync", (process_entry_t)test_sync);

        process_register("endless_loop", (process_entry_t)endless_loop);
        process_register("endless_loop_print", (process_entry_t)endless_loop_print);

        my_create_process("idle", 0, (void *)0);
        shell();
        return 0;
}
