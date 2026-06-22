// semaphores.h
#include <stdint.h>
#include <lib_common.h>
#define MAX_SEM 20
#define MAX_ID_LENGTH 20

void sem_system_init(void);
int64_t sem_open(char* sem_id, uint64_t value);
int64_t sem_post(char* sem_id);
int64_t sem_wait(char* sem_id);
int64_t sem_close(char* sem_id);

/* Remove a dying process (pid is a pid_t) from every semaphore wait queue. */
void sem_remove_from_queues(int64_t pid);