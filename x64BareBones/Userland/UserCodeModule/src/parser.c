#include <parser.h>

// Primera version (MUUUY MEJORABLE)
uint8_t analize_prompt(uint8_t *prompt) {
        for (int i = 0; commands[i] != 0; i++) {
                if (strcmp(prompt, commands[i]) == 0) {
                        // llamar a la funcion que corresponde
                }
        }
}