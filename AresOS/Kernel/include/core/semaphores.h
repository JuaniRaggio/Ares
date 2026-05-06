// semaphores.h
#include <stdint.h>
#define MAX_SEM 20

typedef struct sem sem_t;

int64_t sem_init(uint64_t sem_id, int64_t value);
int64_t sem_post(uint64_t sem_id);
int64_t sem_wait(uint64_t sem_id);
int64_t sem_close(uint64_t sem_id);