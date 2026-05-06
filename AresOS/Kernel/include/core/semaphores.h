// semaphores.h
#include <stdint.h>
#include <lib_common.h>
#define MAX_SEM 20
#define MAX_ID_LENGTH 20

void sem_system_init(void);
int64_t sem_init(char* sem_id, uint64_t value);
int64_t sem_open(char* sem_id);
int64_t sem_post(uint64_t sem_idx);
int64_t sem_wait(uint64_t sem_idx);
int64_t sem_close(uint64_t sem_idx);