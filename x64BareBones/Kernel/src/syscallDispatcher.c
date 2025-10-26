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

uint64_t sys_write(uint32_t fd, const char *buf, uint64_t count) {
        switch (fd) {
        case STDOUT:
                ncPrintCount(buf, 0x0F, count);
                break;
        case STDERR:
                ncPrintCount(buf, 0x04, count);
                break;

        default:
                break;
        }

        return 0;
}