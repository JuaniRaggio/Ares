/* userCodeModule.c */

#include <process_api.h>
#include <shell.h>

extern uint64_t test_mm(uint64_t argc, char *argv[]);
extern uint64_t test_prio(uint64_t argc, char *argv[]);
extern int64_t test_processes(uint64_t argc, char *argv[]);  // Retorna int64_t, no uint64_t
extern uint64_t test_sync(uint64_t argc, char *argv[]);

// Funciones auxiliares de test_util.c
extern void endless_loop(void);
extern void endless_loop_print(uint64_t wait);

// Funciones auxiliares de test_prio.c
extern void zero_to_max(void);

// Funciones auxiliares de test_sync.c
extern uint64_t my_process_inc(uint64_t argc, char *argv[]);

int main() {
        // process_register("idle", (process_entry_t)idle_process);
        //
        // process_register("test_mm", (process_entry_t)test_mm);
        // process_register("test_prio", (process_entry_t)test_prio);
        // process_register("test_processes", (process_entry_t)test_processes);
        // process_register("test_sync", (process_entry_t)test_sync);
        //
        // process_register("endless_loop", (process_entry_t)endless_loop);
        // process_register("endless_loop_print", (process_entry_t)endless_loop_print);
        // process_register("zero_to_max", (process_entry_t)zero_to_max);
        // process_register("my_process_inc", (process_entry_t)my_process_inc);
        //
        // my_create_process("idle", 0, (void *)0);

        shell();
        return 0;
}
