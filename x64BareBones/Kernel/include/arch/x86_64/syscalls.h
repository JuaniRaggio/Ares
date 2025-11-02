// syscalls.h

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

// syscalls IDs
#define SYS_WRITE 0
#define SYS_EXIT 1

// syscalls
uint64_t sys_write(uint64_t fd, const char *buf, uint64_t len);
void sys_exit(uint64_t code);

void init_syscalls(void);

#endif
