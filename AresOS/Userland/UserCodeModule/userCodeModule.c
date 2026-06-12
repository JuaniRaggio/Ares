/* userCodeModule.c */

#include <stddef.h>
#include <apps.h>
#include <process_api.h>
#include <shell.h>
#include <stdint.h>

extern uint64_t test_mm(uint64_t argc, char *argv[]);
extern uint64_t test_prio(uint64_t argc, char *argv[]);
extern int64_t test_processes(uint64_t argc, char *argv[]);
extern uint64_t test_sync(uint64_t argc, char *argv[]);

// Funciones auxiliares de test_util.c
extern void endless_loop(void);
extern void endless_loop_print(uint64_t wait);

// Funciones auxiliares de test_prio.c
extern void zero_to_max(void);

// Funciones auxiliares de test_sync.c
extern uint64_t my_process_inc(uint64_t argc, char *argv[]);

/* Process registration belongs to the userland module init, not to the
 * shell: the shell is just another process that spawns them by name. */
static void register_processes(void) {
        process_register("idle", (process_entry_t)idle_process);

        register_apps();

        process_register("test_mm", (process_entry_t)test_mm);
        process_register("test_prio", (process_entry_t)test_prio);
        process_register("test_proc", (process_entry_t)test_processes);
        process_register("test_sync", (process_entry_t)test_sync);

        process_register("endless_loop", (process_entry_t)endless_loop);
        process_register("endless_loop_print",
                         (process_entry_t)endless_loop_print);
        process_register("zero_to_max", (process_entry_t)zero_to_max);
        process_register("my_process_inc", (process_entry_t)my_process_inc);
}

int main() {
        register_processes();
        my_create_process("idle", 0, NULL);

        shell();
        return 0;
}
