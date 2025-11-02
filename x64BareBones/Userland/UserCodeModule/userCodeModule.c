/* userCodeModule.c */

#include <unistd.h>

int main() {
        // Mensaje de bienvenida
        const char *msg = "USERLAND: Sistema iniciado correctamente\n";
        write(STDOUT_FILENO, msg, 42);

        // TODO: Llamar a la shell cuando este lista
        // shell();

        // Por ahora, loop infinito
        while (1) {
                __asm__ volatile("hlt");
        }

        return 0;
}
