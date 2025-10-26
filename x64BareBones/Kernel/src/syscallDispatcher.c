#include <syscallDispatcher.h>

void syscallDispatcher(uint64_t syscall_id, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    switch (syscall_id) {
        case 4:
            sys_write(arg1, (char *)arg2, arg3);
            break;
    
        default:
            break;
    }
}

uint64_t sys_write(uint32_t fd, const char * buf, uint64_t count) {
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