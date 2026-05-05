#include <semaphores.h>
#include <stddef.h>
#include <interrupts.h>
#include <process.h>

typedef struct pnode{
    pid_t pid;
    struct pnode *next;
}pNode_t;

static sem_t semaphores[MAX_SEM];

void sem_system_init() {
    for (int i = 0; i < MAX_SEM; i++) {
        semaphores[i].value = -1;
        semaphores[i].head = NULL;
        semaphores[i].tail = NULL;
    }
}

static void dequeue_process(uint64_t sem_id){
    if(semaphores[sem_id].head == NULL) return;

    pNode_t *aux = semaphores[sem_id].head;
    semaphores[sem_id].head = aux->next;

    process_unblock(aux->pid);

    // liberar nodo que acabamos de sacar

    if(semaphores[sem_id].head == NULL){
        semaphores[sem_id].tail = NULL;
    }
    return;
}

static void enqueue_process(uint64_t sem_id){
    // creamos nodo
    pNode_t *new_node = algo; 
    if(semaphores[sem_id].tail == NULL){
        semaphores[sem_id].head = new_node;
    }else{
        semaphores[sem_id].tail->next = new_node;
    }
    semaphores[sem_id].tail = new_node;
    return;
}

static void free_queue(uint64_t sem_id){

}

int64_t sem_init(uint64_t sem_id, int64_t value){
    if(sem_id >= MAX_SEM || semaphores[sem_id].value != -1) return -1;
    semaphores[sem_id].value = value;
    return 0;
}

int64_t sem_post(uint64_t sem_id){
    if(sem_id >= MAX_SEM) return -1;
    cli();
    if(semaphores[sem_id].value >= 0){
        if(++(semaphores[sem_id].value) == 1){
            dequeue_process(sem_id);
        }
        sti();
        return 0;
    }else{
        sti();
        return -1;
    }
}
int64_t sem_wait(uint64_t sem_id){
    if(sem_id >= MAX_SEM) return -1;

    cli();
    if(semaphores[sem_id].value >= 0){
        if(semaphores[sem_id].value == 0){
            enqueue_process(sem_id);
            if (process_block(process_getpid()) == -1){
                sti();
                return -1;
            }
            sti();
            schedule();
            cli();
        }
        semaphores[sem_id].value--;
        sti();
        return 0;
    }else{
        sti();
        return -1;
    }
}

int64_t sem_close(uint64_t sem_id){
    if(sem_id >= MAX_SEM) return -1;

    cli();
    sti();

}