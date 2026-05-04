#pragma once

#include <stdint.h>
#include <syscalls.h>

static inline void *malloc(uint64_t size) {
        return syscall_malloc(size);
}

static inline void free(void *ptr) {
        syscall_free(ptr);
}
