#include <lib.h>
#include <stdarg.h>
#include <stdlib.h>
#include <syscall.h>

#define MAX_CHARS 256
#define CURSOR_FREQ 10
#define STDIN 0
#define STDOUT 1

/* Lee un caracter del stdin (bloqueante con polling) */
int getchar(void) {
        char c = 0;
        while (c == 0) {
                syscall_read(STDIN, &c, 1);
        }
        return c;
}

/* Lee input formateado con soporte para backspace y cursor animado */
int scanf(char *fmt, ...) {
        va_list v;
        va_start(v, fmt);

        char c;
        uint64_t ticks   = syscall_get_ticks();
        int cursorTicks  = 0;
        char cursorDrawn = 0;
        char buffer[MAX_CHARS];
        uint64_t bIdx = 0;

        // Loop de lectura con cursor animado
        while ((c = getchar()) != '\n' && bIdx < MAX_CHARS - 1) {
                cursorTicks = syscall_get_ticks() - ticks;

                // Animación del cursor cada CURSOR_FREQ ticks
                if (cursorTicks > CURSOR_FREQ) {
                        ticks       = syscall_get_ticks();
                        cursorTicks = 0;
                        if (cursorDrawn) {
                                putchar('\b');
                        } else {
                                putchar('_');
                        }
                        cursorDrawn = !cursorDrawn;
                }

                if (c != 0) {
                        // Borrar cursor si estaba dibujado
                        if (cursorDrawn) {
                                putchar('\b');
                                cursorDrawn = !cursorDrawn;
                        }

                        // Manejar backspace
                        if (c != '\b') {
                                buffer[bIdx++] = c;
                                putchar(c);
                        } else if (bIdx > 0) {
                                bIdx--;
                                putchar(c);
                        }
                }
        }

        // Limpiar cursor si quedó dibujado
        if (cursorDrawn) {
                putchar('\b');
        }

        putchar('\n');
        buffer[bIdx] = 0;

        // Parsear el formato
        char *fmtPtr = fmt;
        char *end;
        bIdx          = 0;
        int qtyParams = 0;

        while (*fmtPtr && buffer[bIdx] && bIdx < MAX_CHARS) {
                if (*fmtPtr == '%') {
                        fmtPtr++;
                        switch (*fmtPtr) {
                        case 'c':
                                *(char *)va_arg(v, char *) = buffer[bIdx];
                                end                        = &buffer[bIdx] + 1;
                                break;
                        case 's': {
                                char *dest = va_arg(v, char *);
                                int i      = 0;
                                while (buffer[bIdx + i] &&
                                       buffer[bIdx + i] != ' ') {
                                        dest[i] = buffer[bIdx + i];
                                        i++;
                                }
                                dest[i] = 0;
                                end     = &buffer[bIdx + i];
                                break;
                        }
                        }
                        bIdx += end - &buffer[bIdx];
                        qtyParams++;
                } else if (*fmtPtr == buffer[bIdx]) {
                        bIdx++;
                }
                fmtPtr++;
        }

        va_end(v);
        return qtyParams;
}
