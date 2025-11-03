/* _loader.c */
#include <lib.h>

extern char bss;
extern char endOfBinary;

int main();

int _start() {
        memset(&bss, 0, &endOfBinary - &bss);
        return main();
}
