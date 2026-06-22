#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>

typedef volatile uint64_t spinlock_t;

extern void acquire_lock(spinlock_t *lock);
extern void release_lock(spinlock_t *lock);

#endif /* SPINLOCK_H */
