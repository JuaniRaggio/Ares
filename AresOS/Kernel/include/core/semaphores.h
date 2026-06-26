// semaphores.h
#pragma once
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

/* Release every semaphore reference a dying process still held open, freeing
 * the per-process open counts (indexed by semaphore slot, MAX_SEM entries).
 * Called from process_free_resources so a process that dies with semaphores
 * open does not leak their reference counts, the same way its heap allocations
 * and pipes are reclaimed. */
void sem_release_process_refs(uint8_t *open_sems);