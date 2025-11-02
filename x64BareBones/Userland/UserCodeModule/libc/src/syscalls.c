/* syscalls.c - Wrappers de syscalls para userland */

// Declarar _syscall3 (implementado en asm/syscall.asm)
extern long _syscall3(long n, long a1, long a2, long a3);

// Numeros de syscall
#define SYS_WRITE 0
#define SYS_EXIT 1
#define SYS_READ 2

// Wrapper para write
long write(int fd, const void *buf, unsigned long count) {
        return _syscall3(SYS_WRITE, fd, (long)buf, count);
}

// Wrapper para read
long read(int fd, void *buf, unsigned long count) {
        return _syscall3(SYS_READ, fd, (long)buf, count);
}

// Wrapper para exit
void exit(int status) {
        _syscall3(SYS_EXIT, status, 0, 0);
        while (1)
                ; // No deberia llegar aqui
}
