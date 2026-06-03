#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>

typedef volatile uint64_t spinlock_t;

void acquire_lock(spinlock_t *lock);
void release_lock(spinlock_t *lock);

#endif /* SPINLOCK_H */