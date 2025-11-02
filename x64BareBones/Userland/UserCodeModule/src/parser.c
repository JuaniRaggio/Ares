#include <lib.h>
#include <parser.h>
#include <syscall.h>

// Primera version (MUUUY MEJORABLE)
uint8_t analize_prompt(const char *prompt) {
        for (int i = 0; commands[i] != 0; i++) {
                if (strcmp(prompt, commands[i]) == 0) {
                        // llamar a la funcion que corresponde
                        // syscall_write(0, "Hola", 4);
                }
        }
}
