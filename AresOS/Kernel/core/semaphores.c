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
    if(node != NULL){
        free_pNode(node->next);
        unblock_by_semaphore(node->pid);
        slab_free(pNode_cache, node);
    }
    return;
}

static void free_queue(uint64_t sem_id) {
    return free_pNode(semaphores[sem_id].head);
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
            semaphores[sem_idx].value++;
            release_lock(&semaphores[sem_idx].lock);
            _sti();
            return -1;
        }
        release_lock(&semaphores[sem_idx].lock);
        _sti();
        scheduler_yield();
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
