#include <sem_api.h>
#include <syscalls.h>
#include <lib_common.h>

int64_t my_sem_open(char* sem_id, uint64_t value) {
    return syscall_sem_open(sem_id, value);
}

int64_t my_sem_post(char* sem_id) {
    return syscall_sem_post(sem_id);
}

int64_t my_sem_wait(char* sem_id) {
    return syscall_sem_wait(sem_id);
}

int64_t my_sem_close(char* sem_id) {
    return syscall_sem_close(sem_id);
}

