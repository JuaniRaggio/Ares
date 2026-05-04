/* userCodeModule.c */

#include <process_api.h>
#include <shell.h>

int main() {
        process_register("idle", (process_entry_t)idle_process);
        my_create_process("idle", 0, (void *)0);
        shell();
        return 0;
}
