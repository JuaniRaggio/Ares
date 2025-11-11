#include <stdint.h>
#include <stdio.h>

/* Lee un caracter del stdin (non-blocking) */
int getchar(void) {
        char c = 0;
        syscall_read(STDIN, &c, 1);
        return c;
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
                        /* Check for 'll' modifier (long long) */
                        if (*p == 'l' && *(p + 1) == 'l') {
                                p += 2;
                                if (*p == 'u') {
                                        uint64_t num = va_arg(args, uint64_t);
                                        char buf[32];
                                        utoa(num, buf, 10);
                                        char *s = buf;
                                        while (*s) {
                                                putchar(*s++);
                                                count++;
                                        }
                                        p++;
                                        continue;
                                } else if (*p == 'd') {
                                        int64_t num = va_arg(args, int64_t);
                                        char buf[32];
                                        itoa(num, buf, 10);
                                        char *s = buf;
                                        while (*s) {
                                                putchar(*s++);
                                                count++;
                                        }
                                        p++;
                                        continue;
                                }
                        }
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
                        case 'x': {
                                uint64_t num = va_arg(args, uint64_t);
                                char buf[32];
                                utoa(num, buf, 16);
                                int len = 0;
                                char *s = buf;
                                while (*s++)
                                        len++;
                                for (int i = 0; i < 16 - len; i++) {
                                        putchar('0');
                                        count++;
                                }
                                s = buf;
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
