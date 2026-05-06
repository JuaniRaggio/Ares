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
    char id[MAX_ID_LENGTH];
    int64_t value;
    pNode_t *head;
    pNode_t *tail;
};

typedef struct sem sem_t;
static sem_t semaphores[MAX_SEM];
static uint64_t sem_count;

slab_cache_t* pNode_cache = NULL;

void sem_system_init(void) {
    sem_count = 0;
    for (int i = 0; i < MAX_SEM; i++) {
        semaphores[i].value = -1;
        semaphores[i].head  = NULL;
        semaphores[i].tail  = NULL;
        semaphores[i].id[0] = '\0';
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

int64_t search_sem(char* sem_id) {
    for(int i = 0; i < MAX_SEM; i++) {
        if (strcmp(semaphores[i].id, sem_id) == 0)
            return i;
    }
    return -1;
}

uint64_t assign_sem_id(char* sem_id, uint64_t value) {
    for(int i = 0; i < MAX_SEM; i++) {
        if (semaphores[i].value == -1) {
            strcpy(semaphores[i].id, sem_id);
            semaphores[i].value = value;
            sem_count++;
            return i;
        }
    }
    return (uint64_t)-1;
}

int64_t sem_init(char* sem_id, uint64_t value) {
    if (strlen(sem_id) >= MAX_ID_LENGTH || sem_count >= MAX_SEM || value < 0 || search_sem(sem_id) >= 0)
        return -1;
        
    return assign_sem_id(sem_id, value);
}

int64_t sem_open(char* sem_id) {
    if(strlen(sem_id) >= MAX_ID_LENGTH)
        return -1;
    return search_sem(sem_id);
}

int64_t sem_post(uint64_t sem_idx) {
    if (sem_idx >= MAX_SEM)
        return -1;

    cli();

    if (semaphores[sem_idx].value < 0) {
        sti();
        return -1;
    }

    semaphores[sem_idx].value++;
    if (semaphores[sem_idx].value >= 0) dequeue_process(sem_idx);

    sti();
    return 0;
}

int64_t sem_wait(uint64_t sem_idx) {
    if (sem_idx >= MAX_SEM)
        return -1;

    cli();

    if (semaphores[sem_idx].value < 0) {
        sti();
        return -1;
    }

    if (semaphores[sem_idx].value == 0) {
        pid_t pid = process_getpid();
        enqueue_process(sem_idx, pid);
        if (process_block(pid) == -1) {
            semaphores[sem_idx].value++;
            sti();
            return -1;
        }
        cli();
    }

    semaphores[sem_idx].value--;
    sti();
    return 0;
}

int64_t sem_close(uint64_t sem_idx) {
    if (sem_idx >= MAX_SEM)
        return -1;

    cli();

    if (semaphores[sem_idx].value == -1) {
        sti();
        return -1;
    }

    free_queue(sem_idx);
    semaphores[sem_idx].head = NULL;
    semaphores[sem_idx].tail = NULL;
    semaphores[sem_idx].value = -1;
    semaphores[sem_idx].id[0] = '\0';
    sem_count--;

    sti();
    return 0;
}