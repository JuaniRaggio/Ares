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

                remove_pid_from_queue(i, (pid_t)pid);
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

/* Per-process accounting of open semaphores: every successful sem_open by the
 * running process bumps its count for that slot, every sem_close drops it. On
 * death sem_release_process_refs replays the leftover opens so refs never leak.
 */
static void track_sem_open(int64_t sem_idx) {
        pcb_t *cur = process_get_current();
        if (cur != NULL && sem_idx >= 0 && sem_idx < MAX_SEM)
                cur->open_sems[sem_idx]++;
}

static void track_sem_close(int64_t sem_idx) {
        pcb_t *cur = process_get_current();
        if (cur != NULL && sem_idx >= 0 && sem_idx < MAX_SEM &&
            cur->open_sems[sem_idx] > 0)
                cur->open_sems[sem_idx]--;
}

int64_t sem_open(char *sem_id, uint64_t value) {
        if (sem_id == NULL || sem_id[0] == '\0' ||
            strlen(sem_id) >= MAX_ID_LENGTH)
                return SYS_ERR;

        uint64_t flags = irq_save();
        acquire_lock(&semaphores_lock);

        int64_t existing = search_sem(sem_id);
        if (existing >= 0) {
                /* Shared by another process: just take a reference. */
                semaphores[existing].refs++;
                track_sem_open(existing);
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

                        // NULL head and tail cause maybe close
                        // hasn't done it last time
                        semaphores[i].head = NULL;
                        semaphores[i].tail = NULL;

                        sem_count++;
                        track_sem_open(i);
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
        if (sem_id == NULL || sem_id[0] == '\0' ||
            strlen(sem_id) >= MAX_ID_LENGTH)
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

        semaphores[sem_idx].value++;
        pid_t blocked_pid = dequeue_process(sem_idx);
        release_lock(&semaphores[sem_idx].lock);
        if (blocked_pid != NO_PID)
                unblock_by_semaphore(blocked_pid);

        irq_restore(flags);
        return SYS_OK;
}

int64_t sem_wait(char *sem_id) {
        if (sem_id == NULL || sem_id[0] == '\0' ||
            strlen(sem_id) >= MAX_ID_LENGTH)
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

        while (semaphores[sem_idx].value == 0) {
                pid_t pid = process_getpid();
                if (enqueue_process(sem_idx, pid) != SYS_OK) {
                        release_lock(&semaphores[sem_idx].lock);
                        irq_restore(flags);
                        return SYS_ERR;
                }
                if (block_by_semaphore(pid) == SYS_ERR) {
                        remove_pid_from_queue(sem_idx, pid);
                        release_lock(&semaphores[sem_idx].lock);
                        irq_restore(flags);
                        return SYS_ERR;
                }
                release_lock(&semaphores[sem_idx].lock);
                irq_restore(flags);

                /* Sleep until a sem_post wakes us; the _hlt() loop is the
                 * backstop so we never return to userland while still BLOCKED. */
                pcb_t *self = process_get_current();
                _yield_now();
                while (self->state == PROCESS_BLOCKED)
                        _hlt();

                /* Re-acquire the lock and loop to re-test the value. */
                flags = irq_save();
                acquire_lock(&semaphores[sem_idx].lock);
        }

        semaphores[sem_idx].value--;
        release_lock(&semaphores[sem_idx].lock);
        irq_restore(flags);
        return SYS_OK;
}

int64_t sem_close(char *sem_id) {
        if (sem_id == NULL || sem_id[0] == '\0' ||
            strlen(sem_id) >= MAX_ID_LENGTH)
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
        track_sem_close(sem_idx);

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

void sem_release_process_refs(uint8_t *open_sems) {
        if (open_sems == NULL)
                return;

        uint64_t flags = irq_save();
        acquire_lock(&semaphores_lock);

        for (int i = 0; i < MAX_SEM; i++) {
                uint8_t cnt   = open_sems[i];
                open_sems[i]  = 0;
                if (cnt == 0 || semaphores[i].id[0] == '\0')
                        continue;

                acquire_lock(&semaphores[i].lock);

                /* Drop the references this process never closed; the last one
                 * destroys the semaphore exactly like sem_close would. */
                while (cnt-- > 0 && semaphores[i].refs > 0)
                        semaphores[i].refs--;

                if (semaphores[i].refs == 0) {
                        free_queue(i);
                        semaphores[i].head  = NULL;
                        semaphores[i].tail  = NULL;
                        semaphores[i].value = 0;
                        semaphores[i].id[0] = '\0';
                        sem_count--;
                }

                release_lock(&semaphores[i].lock);
        }

        release_lock(&semaphores_lock);
        irq_restore(flags);
}

