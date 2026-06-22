#pragma once
/**
 * @file sem_api.h
 * @brief Userland semaphore API.
 *
 * Provides the my_sem_* functions expected by the test suite, which
 * internally call the corresponding syscalls.
 */

#include <stdint.h>

/**
 * @brief Create or open a semaphore with the given ID and initial value.
 * @param sem_id Null-terminated string ID for the semaphore (max length 20).
 * @param value Initial value for the semaphore.
 * @return 1 on success, 0 on failure (e.g. invalid ID, too many semaphores).
 */
int64_t my_sem_open(char* sem_id, uint64_t value);

/**
 * @brief Post (signal) a semaphore by ID.
 * @param sem_id Null-terminated string ID of the semaphore to post.
 * @return 0 on success, -1 on failure (e.g. invalid ID).
 */
int64_t my_sem_post(char* sem_id);

/**
 * @brief Wait (decrement) a semaphore by ID, blocking if the value is zero.
 * @param sem_id Null-terminated string ID of the semaphore to wait on.
 * @return 0 on success, -1 on failure (e.g. invalid ID).
 */
int64_t my_sem_wait(char* sem_id);

/**
 * @brief Close a semaphore by ID, freeing its resources.
 * @param sem_id Null-terminated string ID of the semaphore to close.
 * @return 0 on success, -1 on failure (e.g. invalid ID).
 */ 
int64_t my_sem_close(char* sem_id);
