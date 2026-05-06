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
#define NULL ((void*)0)

typedef struct pnode {
    pid_t pid;
    struct pnode *next;
} pNode_t;

struct sem {
    int64_t value;
    pNode_t *head;
    pNode_t *tail;
};

typedef struct sem sem_t;
static sem_t semaphores[MAX_SEM];

slab_cache_t* pNode_cache = NULL;

void sem_system_init(void) {
    for (int i = 0; i < MAX_SEM; i++) {
        semaphores[i].value = -1;
        semaphores[i].head  = NULL;
        semaphores[i].tail  = NULL;
    }

    pNode_cache = create_cache(sizeof(pNode_t));
}

static void dequeue_process(uint64_t sem_id) {
    pNode_t *node = semaphores[sem_id].head;
    if (node == NULL) return;

    semaphores[sem_id].head = node->next;
    process_unblock(node->pid);

    if (semaphores[sem_id].head == NULL) 
        semaphores[sem_id].tail = NULL;

    slab_free(pNode_cache, node);
    
}

static void enqueue_process(uint64_t sem_id, pid_t pid) {
    pNode_t *new_node = slab_alloc(pNode_cache);
    new_node->pid = pid;
    new_node->next = (void *)0;

    if(semaphores[sem_id].head == NULL) 
        semaphores[sem_id].head = new_node;
    else semaphores[sem_id].tail->next = new_node;

    semaphores[sem_id].tail = new_node;
    return;
}

static void free_pNode(pNode_t *node){
    if(node != NULL){
        free_pNode(node->next);
        process_unblock(node->pid);
        slab_free(pNode_cache, node);
    }
    return;
}

static void free_queue(uint64_t sem_id) {
    return free_pNode(semaphores[sem_id].head);
}


int64_t sem_init(uint64_t sem_id, uint64_t value) {
    if (sem_id >= MAX_SEM)
        return -1;
    if (semaphores[sem_id].value != -1)
        return -1;

    semaphores[sem_id].value = value;
    semaphores[sem_id].head  = NULL;
    semaphores[sem_id].tail  = NULL;
    return 0;
}

int64_t sem_post(uint64_t sem_id) {
    if (sem_id >= MAX_SEM)
        return -1;

    cli();

    if (semaphores[sem_id].value < 0) {
        sti();
        return -1;
    }

    semaphores[sem_id].value++;
    if (semaphores[sem_id].value >= 0) dequeue_process(sem_id);

    sti();
    return 0;
}

int64_t sem_wait(uint64_t sem_id) {
    if (sem_id >= MAX_SEM)
        return -1;

    cli();

    if (semaphores[sem_id].value < 0) {
        sti();
        return -1;
    }

    if (semaphores[sem_id].value == 0) {
        pid_t pid = process_getpid();
        enqueue_process(sem_id, pid);
        if (process_block(pid) == -1) {
            semaphores[sem_id].value++;
            sti();
            return -1;
        }
        cli();
    }

    semaphores[sem_id].value--;
    sti();
    return 0;
}

int64_t sem_close(uint64_t sem_id) {
    if (sem_id >= MAX_SEM)
        return -1;

    cli();

    if (semaphores[sem_id].value == -1) {
        sti();
        return -1;
    }

    free_queue(sem_id);
    semaphores[sem_id].head = NULL;
    semaphores[sem_id].tail = NULL;
    semaphores[sem_id].value = -1;

    sti();
    return 0;
}