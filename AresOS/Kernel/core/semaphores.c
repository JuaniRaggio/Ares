// semaphores.c
#include <interrupts.h>
#include <lib_common.h>
#include <process.h>
#include <scheduler.h>
#include <semaphores.h>
#include <slab.h>
#include <spinlock.h>
#include <status_codes.h>
#include <stddef.h>
#include <stdint.h>

/* IMPORTANT: irq_save (not _cli/_sti): this also runs from the keyboard IRQ via
 * process_kill_foreground (Ctrl+C), where interrupts are already off
 * and a bare _sti would wrongly re-enable them inside the handler. */

/* search_sem() returns a table index; this sentinel means "no such id". */
#define SEM_NOT_FOUND (-1)

typedef struct pnode {
        pid_t pid;
        struct pnode *next;
} pNode_t;

struct sem {
        char id[MAX_ID_LENGTH];
        int64_t value;
        spinlock_t lock;
        pNode_t *head;
        pNode_t *tail;
        uint64_t refs;
};

typedef struct sem sem_t;
static sem_t semaphores[MAX_SEM];
static uint64_t sem_count;
static spinlock_t semaphores_lock;

static slab_cache_t *pNode_cache = NULL;

void sem_system_init(void) {
        sem_count       = 0;
        semaphores_lock = 0;
        for (int i = 0; i < MAX_SEM; i++) {
                semaphores[i].lock  = 0;
                semaphores[i].head  = NULL;
                semaphores[i].tail  = NULL;
                semaphores[i].id[0] = '\0';
        }

        pNode_cache = create_cache(sizeof(pNode_t));
}

static pid_t dequeue_process(uint64_t sem_id) {
        pNode_t *node = semaphores[sem_id].head;
        if (node == NULL)
                return NO_PID;

        pid_t pid               = node->pid;
        semaphores[sem_id].head = node->next;

        slab_free(pNode_cache, node);

        if (semaphores[sem_id].head == NULL)
                semaphores[sem_id].tail = NULL;

        return pid;
}

static sys_status_t enqueue_process(uint64_t sem_id, pid_t pid) {
        pNode_t *new_node = slab_alloc(pNode_cache);
        if (new_node == NULL)
                return SYS_ERR; /* out of memory: caller must undo the value
                                   decrement */
        new_node->pid  = pid;
        new_node->next = NULL;

        if (semaphores[sem_id].head == NULL)
                semaphores[sem_id].head = new_node;
        else
                semaphores[sem_id].tail->next = new_node;

        semaphores[sem_id].tail = new_node;
        return SYS_OK;
}

static void free_pNode(pNode_t *node) {
        /* Iterative: a deep wait queue would otherwise recurse one frame per
         * blocked process and could overflow the kernel stack. */
        while (node != NULL) {
                pNode_t *next = node->next;
                unblock_by_semaphore(node->pid);
                slab_free(pNode_cache, node);
                node = next;
        }
}

static void free_queue(uint64_t sem_id) {
        return free_pNode(semaphores[sem_id].head);
}

/* Removes a single pid from a semaphore's wait queue. Returns 1 if found. */
static int remove_pid_from_queue(uint64_t sem_id, pid_t pid) {
        pNode_t *prev = NULL;
        pNode_t *cur  = semaphores[sem_id].head;
        while (cur != NULL) {
                if (cur->pid == pid) {
                        if (prev == NULL)
                                semaphores[sem_id].head = cur->next;
                        else
                                prev->next = cur->next;
                        if (semaphores[sem_id].tail == cur)
                                semaphores[sem_id].tail = prev;
                        slab_free(pNode_cache, cur);
                        return 1;
                }
                prev = cur;
                cur  = cur->next;
        }
        return 0;
}

void sem_remove_from_queues(int64_t pid) {
        uint64_t flags = irq_save();
        acquire_lock(&semaphores_lock);
        for (int i = 0; i < MAX_SEM; i++) {
                if (semaphores[i].id[0] == '\0')
                        continue;
                acquire_lock(&semaphores[i].lock);
                if (remove_pid_from_queue(i, (pid_t)pid))
                        semaphores[i].value++;
                release_lock(&semaphores[i].lock);
        }
        release_lock(&semaphores_lock);
        irq_restore(flags);
}

int64_t search_sem(char *sem_id) {
        for (int i = 0; i < MAX_SEM; i++) {
                if (strcmp(semaphores[i].id, sem_id) == 0)
                        return i;
        }
        return SEM_NOT_FOUND;
}

int64_t sem_open(char *sem_id, uint64_t value) {
        if (strlen(sem_id) >= MAX_ID_LENGTH)
                return SYS_ERR;

        uint64_t flags = irq_save();
        acquire_lock(&semaphores_lock);

        int64_t existing = search_sem(sem_id);
        if (existing >= 0) {
                /* Shared by another process: just take a reference. */
                semaphores[existing].refs++;
                release_lock(&semaphores_lock);
                irq_restore(flags);
                return SYS_OK;
        }

        if (sem_count >= MAX_SEM) {
                release_lock(&semaphores_lock);
                irq_restore(flags);
                return SYS_ERR;
        }

        for (int i = 0; i < MAX_SEM; i++) {
                if (strcmp(semaphores[i].id, "\0") == 0) {
                        strcpy(semaphores[i].id, sem_id);
                        semaphores[i].value = value;
                        semaphores[i].lock  = 0;
                        semaphores[i].refs  = 1;
                        sem_count++;
                        release_lock(&semaphores_lock);
                        irq_restore(flags);
                        return SYS_OK;
                }
        }
        release_lock(&semaphores_lock);
        irq_restore(flags);
        return SYS_ERR;
}

int64_t sem_post(char *sem_id) {
        if (strlen(sem_id) >= MAX_ID_LENGTH)
                return SYS_ERR;

        uint64_t flags = irq_save();

        acquire_lock(&semaphores_lock);
        int64_t sem_idx = search_sem(sem_id);

        if (sem_idx < 0) {
                release_lock(&semaphores_lock);
                irq_restore(flags);
                return SYS_ERR;
        }

        acquire_lock(&semaphores[sem_idx].lock);
        release_lock(&semaphores_lock);

        if (++semaphores[sem_idx].value <= 0) {
                pid_t blocked_pid = dequeue_process(sem_idx);
                release_lock(&semaphores[sem_idx].lock);
                if (blocked_pid != NO_PID)
                        unblock_by_semaphore(blocked_pid);
        } else
                release_lock(&semaphores[sem_idx].lock);

        irq_restore(flags);
        return SYS_OK;
}

int64_t sem_wait(char *sem_id) {
        if (strlen(sem_id) >= MAX_ID_LENGTH)
                return SYS_ERR;

        uint64_t flags = irq_save();
        acquire_lock(&semaphores_lock);

        int64_t sem_idx = search_sem(sem_id);

        if (sem_idx < 0) {
                release_lock(&semaphores_lock);
                irq_restore(flags);
                return SYS_ERR;
        }

        acquire_lock(&semaphores[sem_idx].lock);
        release_lock(&semaphores_lock);

        if (--semaphores[sem_idx].value < 0) {
                pid_t pid = process_getpid();
                if (enqueue_process(sem_idx, pid) != SYS_OK) {
                        semaphores[sem_idx]
                            .value++; /* undo: could not enqueue */
                        release_lock(&semaphores[sem_idx].lock);
                        irq_restore(flags);
                        return SYS_ERR;
                }

                if (block_by_semaphore(pid) == SYS_ERR) {
                        remove_pid_from_queue(sem_idx, pid);
                        semaphores[sem_idx].value++;
                        release_lock(&semaphores[sem_idx].lock);
                        irq_restore(flags);
                        return SYS_ERR;
                }
                release_lock(&semaphores[sem_idx].lock);
                irq_restore(flags);

                /* Block for real. scheduler_yield() only drops the quantum, so
                 * the actual switch happens on the next tick; we must NOT
                 * return to userland while still BLOCKED (the process would
                 * keep running and could re-enter sem_wait, double-enqueueing
                 * its pid). Sleep on _hlt() until sem_post (or a kill) clears
                 * the BLOCKED state, the same pattern process_wait uses. */
                pcb_t *self = process_get_current();
                scheduler_yield();
                while (self->state == PROCESS_BLOCKED)
                        _hlt();
        } else {
                release_lock(&semaphores[sem_idx].lock);
                irq_restore(flags);
        }
        return SYS_OK;
}

int64_t sem_close(char *sem_id) {
        if (strlen(sem_id) >= MAX_ID_LENGTH)
                return SYS_ERR;

        uint64_t flags = irq_save();
        acquire_lock(&semaphores_lock);

        int64_t sem_idx = search_sem(sem_id);
        if (sem_idx < 0) {
                release_lock(&semaphores_lock);
                irq_restore(flags);
                return SYS_ERR;
        }

        acquire_lock(&semaphores[sem_idx].lock);

        /* Only the last close destroys the semaphore; otherwise processes still
         * sharing it would lose their mutex (this is what made test_sync race).
         */
        if (semaphores[sem_idx].refs > 0)
                semaphores[sem_idx].refs--;

        if (semaphores[sem_idx].refs > 0) {
                release_lock(&semaphores[sem_idx].lock);
                release_lock(&semaphores_lock);
                irq_restore(flags);
                return SYS_OK;
        }

        free_queue(sem_idx);
        semaphores[sem_idx].head  = NULL;
        semaphores[sem_idx].tail  = NULL;
        semaphores[sem_idx].value = 0;
        semaphores[sem_idx].id[0] = '\0';

        release_lock(&semaphores[sem_idx].lock);
        sem_count--;
        release_lock(&semaphores_lock);
        irq_restore(flags);

        return SYS_OK;
}
