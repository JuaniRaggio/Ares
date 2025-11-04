#include <stdint.h>
#include <stdio.h>

#define MAX_CHARS 256
#define EOF (-1)
#define CURSOR_FREQ 10

typedef enum {
        STDIN = 0,
        STDOUT,
        STDERR,
} stdcodes;

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

        while ((c = getchar()) != '\n' && bIdx < MAX_CHARS - 1) {
                cursorTicks = syscall_get_ticks() - ticks;

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
                        if (cursorDrawn) {
                                putchar('\b');
                                cursorDrawn = !cursorDrawn;
                        }

                        if (c != '\b') {
                                buffer[bIdx++] = c;
                                putchar(c);
                        } else if (bIdx > 0) {
                                bIdx--;
                                putchar(c);
                        }
                }
        }

        if (cursorDrawn) {
                putchar('\b');
        }

        putchar('\n');
        buffer[bIdx] = 0;

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

int putchar(int c) {
        char ch = (char)c;
        if (syscall_write(STDOUT, &ch, 1) == 1)
                return c;
        return EOF;
}

int puts(const char *s) {
        while (*s) {
                if (putchar(*s++) == EOF)
                        return EOF;
        }
        if (putchar('\n') == EOF)
                return EOF;
        return 0;
}

int printf(const char *format, ...) {
        va_list args;
        va_start(args, format);
        const char *p = format;
        int count     = 0;
        while (*p) {
                if (*p == '%') {
                        p++;
                        switch (*p) {
                        case 'c': {
                                char c = (char)va_arg(args, int);
                                putchar(c);
                                count++;
                                break;
                        }
                        case 's': {
                                char *s = va_arg(args, char *);
                                while (*s) {
                                        putchar(*s++);
                                        count++;
                                }
                                break;
                        }
                        case 'd': {
                                int num = va_arg(args, int);
                                char buf[20];
                                itoa(num, buf, 10);
                                char *s = buf;
                                while (*s) {
                                        putchar(*s++);
                                        count++;
                                }
                                break;
                        }
                        default:
                                putchar('%');
                                putchar(*p);
                                count += 2;
                        }
                } else {
                        putchar(*p);
                        count++;
                }
                p++;
        }
        va_end(args);
        return count;
}
