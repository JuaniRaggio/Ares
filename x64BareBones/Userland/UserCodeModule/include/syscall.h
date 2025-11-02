#pragma once

#include <stdint.h>

uint64_t _syscall3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3);

#define SYS_WRITE 0
#define SYS_EXIT 1

// Wrapper userland -> syscall del kernel
static inline uint64_t syscall_write(uint64_t fd, const char *buf, uint64_t len) {
        return _syscall3(SYS_WRITE, fd, (uint64_t)buf, len);
}

static inline void syscall_exit(int code) {
        _syscall3(SYS_EXIT, code, 0, 0);
}
