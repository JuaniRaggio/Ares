//semaphores.c
#include <semaphores.h>
#include <stddef.h>
#include <lib_common.h>
#include <scheduler.h>
#include <semaphores.h>
#include <interrupts.h>
#include <process.h>
#include <stddef.h>
#include <slab.h>
#include <spinlock.h>


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
};

typedef struct sem sem_t;
static sem_t semaphores[MAX_SEM];
static uint64_t sem_count;
static spinlock_t semaphores_lock;

slab_cache_t* pNode_cache = NULL;

void sem_system_init(void) {
    sem_count = 0;
    semaphores_lock = 0;
    for (int i = 0; i < MAX_SEM; i++) {
        semaphores[i].lock = 0;
        semaphores[i].head  = NULL;
        semaphores[i].tail  = NULL;
        semaphores[i].id[0] = '\0';
    }

    pNode_cache = create_cache(sizeof(pNode_t));
}

static pid_t dequeue_process(uint64_t sem_id) {
    pNode_t *node = semaphores[sem_id].head;
    if (node == NULL) return -1;

    pid_t pid = node->pid;
    semaphores[sem_id].head = node->next;

    slab_free(pNode_cache, node);

    if (semaphores[sem_id].head == NULL) semaphores[sem_id].tail = NULL;

    return pid;
}

static void enqueue_process(uint64_t sem_id, pid_t pid) {
    pNode_t *new_node = slab_alloc(pNode_cache);
    new_node->pid = pid;
    new_node->next = NULL;

    if(semaphores[sem_id].head == NULL) 
        semaphores[sem_id].head = new_node;
    else semaphores[sem_id].tail->next = new_node;

    semaphores[sem_id].tail = new_node;
    return;
}

static void free_pNode(pNode_t *node){
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

/* Drops a (dying) process from every semaphore wait queue. Without this,
 * killing a process blocked in sem_wait leaves a stale node: a later
 * sem_post would dequeue the dead pid, fail to wake it, and swallow the
 * post -- starving the next real waiter. Undoing the dead process's
 * decrement (value++) keeps the counter consistent with the queue. */
void sem_remove_from_queues(int64_t pid) {
    /* irq_save (not _cli/_sti): this also runs from the keyboard IRQ via
     * process_kill_foreground (Ctrl+C), where interrupts are already off
     * and a bare _sti would wrongly re-enable them inside the handler. */
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

int64_t search_sem(char* sem_id) {
    for(int i = 0; i < MAX_SEM; i++) {
        if (strcmp(semaphores[i].id, sem_id) == 0)
            return i;
    }
    return -1;
}

int64_t sem_open(char* sem_id, uint64_t value) {
    if (strlen(sem_id) >= MAX_ID_LENGTH || sem_count >= MAX_SEM )
        return 0;

    _cli();
    acquire_lock(&semaphores_lock);

    if(search_sem(sem_id) >= 0){
        release_lock(&semaphores_lock);
        _sti();
        return 1;
    }

    for(int i = 0; i < MAX_SEM; i++) {
        if (strcmp(semaphores[i].id, "\0") == 0) {
            strcpy(semaphores[i].id, sem_id);
            semaphores[i].value = value;
            semaphores[i].lock = 0;
            sem_count++;
            release_lock(&semaphores_lock);
            _sti();
            return 1;
        }
    }
    release_lock(&semaphores_lock);
    _sti();
    return 0;
}


int64_t sem_post(char* sem_id) {
    if (strlen(sem_id) >= MAX_ID_LENGTH)
        return -1;
        
    _cli();
    
    acquire_lock(&semaphores_lock);
    int64_t sem_idx = search_sem(sem_id);

    if(sem_idx < 0){
        release_lock(&semaphores_lock);
        _sti();
        return -1;
    }

    acquire_lock(&semaphores[sem_idx].lock);
    release_lock(&semaphores_lock);

    if(++semaphores[sem_idx].value <= 0){
        pid_t blocked_pid = dequeue_process(sem_idx);
        release_lock(&semaphores[sem_idx].lock);
        if(blocked_pid != -1) unblock_by_semaphore(blocked_pid);
    }else release_lock(&semaphores[sem_idx].lock);

    _sti();
    return 0;
}

int64_t sem_wait(char* sem_id) {
    if (strlen(sem_id) >= MAX_ID_LENGTH )
        return -1;

    _cli();
    acquire_lock(&semaphores_lock);

    int64_t sem_idx = search_sem(sem_id);

    if(sem_idx < 0) {
        release_lock(&semaphores_lock);
        _sti();
        return -1;
    }

    acquire_lock(&semaphores[sem_idx].lock);
    release_lock(&semaphores_lock);

    if (--semaphores[sem_idx].value < 0) {
        pid_t pid = process_getpid();
        enqueue_process(sem_idx, pid);

        if (block_by_semaphore(pid) == -1) {
            remove_pid_from_queue(sem_idx, pid);
            semaphores[sem_idx].value++;
            release_lock(&semaphores[sem_idx].lock);
            _sti();
            return -1;
        }
        release_lock(&semaphores[sem_idx].lock);
        _sti();

        /* Block for real. scheduler_yield() only drops the quantum, so the
         * actual switch happens on the next tick; we must NOT return to
         * userland while still BLOCKED (the process would keep running and
         * could re-enter sem_wait, double-enqueueing its pid). Sleep on
         * _hlt() until sem_post (or a kill) clears the BLOCKED state, the
         * same pattern process_wait uses. */
        pcb_t *self = process_get_current();
        scheduler_yield();
        while (self->state == PROCESS_BLOCKED)
            _hlt();
    }else{
        release_lock(&semaphores[sem_idx].lock);
        _sti();
    }
    return 0;
}

int64_t sem_close(char* sem_id) {
    if (strlen(sem_id) >= MAX_ID_LENGTH) return -1;

    _cli();
    acquire_lock(&semaphores_lock);

    int64_t sem_idx = search_sem(sem_id);
    if(sem_idx < 0){
        release_lock(&semaphores_lock);
        _sti();
        return 0;
    }

    acquire_lock(&semaphores[sem_idx].lock);

    free_queue(sem_idx);
    semaphores[sem_idx].head = NULL;
    semaphores[sem_idx].tail = NULL;
    semaphores[sem_idx].value = 0;
    semaphores[sem_idx].id[0] = '\0';

    release_lock(&semaphores[sem_idx].lock);
    sem_count--;
    release_lock(&semaphores_lock);
    _sti();

    return 1;
}
