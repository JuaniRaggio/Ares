#ifndef _SYSCALL_DISPATCHER_H_
#define _SYSCALL_DISPATCHER_H_

#include <naiveConsole.h>
#include <stdint.h>

#define STDOUT 1
#define STDERR 2

void syscallDispatcher(uint64_t syscall_id, uint64_t arg1, uint64_t arg2,
                       uint64_t arg3);

extern uint64_t sys_write(uint32_t fd, const char *buf, uint64_t count);

#endif