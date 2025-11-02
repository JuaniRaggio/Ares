/* userCodeModule.c */

#include <shell.h>
#include <syscall.h>

int main() {
        // DEBUG: Verificar que llegamos a userland
        const char *msg = "USERLAND: main() ejecutandose\n";
        syscall_write(1, msg, 31);

        // Temporalmente comentado - usa funciones del kernel que no estan
        // linkeadas shell();

        // Infinite loop para ver el mensaje
        while (1)
                ;

        return 0;
}
