#include <stdio.h>
#include <stdint.h>

/*
 * Lee un caracter del stdin (bloqueante)
 * Se bloquea hasta que haya datos. Retorna EOF si el stream termino
 * (Ctrl+D en teclado o pipe sin escritores)
 */
int getchar(void) {
        char c         = 0;
        uint64_t count = 1;
        syscall_read(STDIN, &c, &count);
        if (count == 0) {
                return EOF;
        }
        return (uint8_t)c;
}

/*
 * Lee un caracter del teclado sin bloquear (para juegos / polling)
 * Retorna 0 si no hay datos disponibles, o el caracter leido
 */
int getchar_nonblock(void) {
        char c         = 0;
        uint64_t count = 1;
        syscall_read(FD_KBD_NONBLOCK, &c, &count);
        if (count == 0) {
                return 0; /* No data available */
        }
        return (uint8_t)c;
}

int putchar(int c) {
        char ch = (char)c;
        if (syscall_write(STDOUT, &ch, 1) == 1)
                return c;
        return EOF;
}

/*
 * Output buffer: printf/puts accumulate here and flush with a single
 * syscall_write, instead of one syscall per character. The boot banner
 * alone is several KB, so this turns thousands of ring transitions into
 * a handful.
 */
#define OUT_BUF_SIZE 256

typedef struct {
        char data[OUT_BUF_SIZE];
        int len;
} out_buf_t;

static void ob_flush(out_buf_t *ob) {
        if (ob->len > 0) {
                syscall_write(STDOUT, ob->data, ob->len);
                ob->len = 0;
        }
}

static void ob_putc(out_buf_t *ob, char c) {
        if (ob->len == OUT_BUF_SIZE)
                ob_flush(ob);
        ob->data[ob->len++] = c;
}

static void ob_puts(out_buf_t *ob, const char *s) {
        while (*s)
                ob_putc(ob, *s++);
}

int puts(const char *s) {
        out_buf_t ob = {.len = 0};
        ob_puts(&ob, s);
        ob_putc(&ob, '\n');
        ob_flush(&ob);
        return 0;
}

int printf(const char *format, ...) {
        va_list args;
        va_start(args, format);
        out_buf_t ob  = {.len = 0};
        const char *p = format;
        int count     = 0;
        char buf[32];
        while (*p) {
                if (*p == '%') {
                        p++;
                        /* Check for 'll' modifier (long long) */
                        if (*p == 'l' && *(p + 1) == 'l') {
                                p += 2;
                                if (*p == 'u') {
                                        utoa(va_arg(args, uint64_t), buf, 10);
                                        ob_puts(&ob, buf);
                                        p++;
                                        continue;
                                } else if (*p == 'd') {
                                        itoa(va_arg(args, int64_t), buf, 10);
                                        ob_puts(&ob, buf);
                                        p++;
                                        continue;
                                }
                        }
                        /* Check for 'l' modifier (long) */
                        else if (*p == 'l') {
                                p++;
                                if (*p == 'u') {
                                        utoa(va_arg(args, unsigned long), buf,
                                             10);
                                        ob_puts(&ob, buf);
                                        p++;
                                        continue;
                                } else if (*p == 'd') {
                                        itoa(va_arg(args, long), buf, 10);
                                        ob_puts(&ob, buf);
                                        p++;
                                        continue;
                                }
                        }
                        switch (*p) {
                        case 'c': {
                                ob_putc(&ob, (char)va_arg(args, int));
                                count++;
                                break;
                        }
                        case 's': {
                                char *s = va_arg(args, char *);
                                while (*s) {
                                        ob_putc(&ob, *s++);
                                        count++;
                                }
                                break;
                        }
                        case 'd': {
                                itoa(va_arg(args, int), buf, 10);
                                ob_puts(&ob, buf);
                                break;
                        }
                        case 'x': {
                                utoa(va_arg(args, uint64_t), buf, 16);
                                int len = 0;
                                while (buf[len])
                                        len++;
                                for (int i = 0; i < 16 - len; i++) {
                                        ob_putc(&ob, '0');
                                        count++;
                                }
                                ob_puts(&ob, buf);
                                break;
                        }
                        default:
                                ob_putc(&ob, '%');
                                ob_putc(&ob, *p);
                                count += 2;
                        }
                } else {
                        ob_putc(&ob, *p);
                        count++;
                }
                p++;
        }
        ob_flush(&ob);
        va_end(args);
        return count;
}
