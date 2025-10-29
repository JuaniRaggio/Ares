#include <syscallDispatcher.h>

void syscallDispatcher(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx) {
        switch (rax) {
        case 4:
                sys_write(rbx, (char *)rcx, rdx);
                break;

        default:
                break;
        }
}

void sys_write(uint32_t fd, const char *buf, uint64_t count) {
        if(fd == STDOUT || fd == STDERR) {
                for(int i = 0; i < count; i++) {
                        //TODO:
                }
        }
}