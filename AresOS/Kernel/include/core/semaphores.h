// semaphores.h
#include <stdint.h>
#define MAX_SEM 20

typedef uint64_t sem_id_t;

void sem_system_init(void);
int64_t sem_init(sem_id_t sem_id, uint64_t value);
int64_t sem_post(sem_id_t sem_id);
int64_t sem_wait(sem_id_t sem_id);
int64_t sem_close(sem_id_t sem_id);