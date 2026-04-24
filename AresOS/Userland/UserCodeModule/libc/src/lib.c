#include <lib.h>
#include <stdint.h>

/* Reverse a string in place */
static void reverse(char *str, int length) {
        int start = 0;
        int end   = length - 1;
        while (start < end) {
                char temp  = str[start];
                str[start] = str[end];
                str[end]   = temp;
                start++;
                end--;
        }
}

char *itoa(int value, char *str, int base) {
        if (base < 2 || base > 36) {
                *str = '\0';
                return str;
        }
        int i           = 0;
        int is_negative = 0;

        if (value == 0) {
                str[i++] = '0';
                str[i]   = '\0';
                return str;
        }

        if (value < 0 && base == 10) {
                is_negative = 1;
                value       = -value;
        }

        while (value != 0) {
                int rem  = value % base;
                str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
                value    = value / base;
        }

        if (is_negative) {
                str[i++] = '-';
        }

        str[i] = '\0';

        reverse(str, i);

        return str;
}

char *utoa(uint64_t value, char *str, int base) {
        char *ptr  = str;
        char *ptr1 = str;
        char tmp_char;
        uint64_t tmp_value;

        if (base < 2 || base > 36) {
                *str = '\0';
                return str;
        }

        if (value == 0) {
                *ptr++ = '0';
                *ptr   = '\0';
                return str;
        }

        while (value != 0) {
                tmp_value = value % base;
                *ptr++ =
                    (tmp_value < 10) ? tmp_value + '0' : tmp_value - 10 + 'a';
                value /= base;
        }

        *ptr-- = '\0';

        while (ptr1 < ptr) {
                tmp_char = *ptr;
                *ptr--   = *ptr1;
                *ptr1++  = tmp_char;
        }

        return str;
}

uint64_t strtoul(const char *str, char **endptr, int base) {
        uint64_t result = 0;
        const char *s   = str;

        while (*s == ' ' || *s == '\t' || *s == '\n')
                s++;

        if (base == 16 || base == 0) {
                if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
                        s += 2;
                        base = 16;
                }
        }

        if (base == 0)
                base = 10;

        while (*s) {
                int digit;
                if (*s >= '0' && *s <= '9')
                        digit = *s - '0';
                else if (*s >= 'a' && *s <= 'f')
                        digit = *s - 'a' + 10;
                else if (*s >= 'A' && *s <= 'F')
                        digit = *s - 'A' + 10;
                else
                        break;

                if (digit >= base)
                        break;

                result = result * base + digit;
                s++;
        }

        if (endptr)
                *endptr = (char *)s;

        return result;
}
