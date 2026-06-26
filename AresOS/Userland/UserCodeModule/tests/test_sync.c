#include <stddef.h>
#include "test_util.h"
#include <process_api.h>
#include <sem_api.h>
#include <stdint.h>
#include <stdio.h>

#define SEM_ID "sem"
/* 2*pairs + shell + idle + launcher fit in the 64-slot process table. */
#define MAX_SYNC_PROCESSES 60

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
        uint64_t aux = *p;
        if (GetUniform(100) < 30)
                my_yield(); // This makes the race condition highly probable
        aux += inc;
        *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
        uint64_t n;
        int8_t inc;
        int8_t use_sem;

        if (argc != 3)
                return -1;

        if ((n = satoi(argv[0])) <= 0)
                return -1;
        if ((inc = satoi(argv[1])) == 0)
                return -1;
        if ((use_sem = satoi(argv[2])) < 0)
                return -1;

        if (use_sem)
                if (!my_sem_open(SEM_ID, 1)) {
                        printf("test_sync: ERROR opening semaphore\n");
                        return -1;
                }

        uint64_t i;
        for (i = 0; i < n; i++) {
                if (use_sem)
                        my_sem_wait(SEM_ID);
                slowInc(&global, inc);
                if (use_sem)
                        my_sem_post(SEM_ID);
        }

        if (use_sem)
                my_sem_close(SEM_ID);

        return 0;
}

/* test_sync <pairs> <n> <use_sem>: spawn `pairs` incrementer/decrementer pairs,
 * each doing n +/-1 operations on the shared `global`. With semaphores the final
 * value must be 0; without them it races. */
uint64_t test_sync(uint64_t argc, char *argv[]) {
        if (argc != 3) {
                printf("Usage: test_sync <pairs> <n> <use_sem>\n");
                return -1;
        }

        int64_t pairs   = satoi(argv[0]);
        int64_t n       = satoi(argv[1]);
        int64_t use_sem = satoi(argv[2]);
        if (pairs <= 0 || n <= 0 || use_sem < 0 || use_sem > 1) {
                printf("Usage: test_sync <pairs> <n> <use_sem>\n");
                return -1;
        }
        if (2 * pairs > MAX_SYNC_PROCESSES) {
                printf("test_sync: too many pairs (max %d)\n",
                       MAX_SYNC_PROCESSES / 2);
                return -1;
        }

        /* Reset the shared semaphore: drain any references leaked by a previous
         * run that was Ctrl+C'd mid-critical-section, then (re)create it fresh at
         * value 1, held by this launcher for the whole run (so-tp2's pattern). */
        if (use_sem) {
                while (my_sem_close(SEM_ID) == 0)
                        ;
                if (!my_sem_open(SEM_ID, 1)) {
                        printf("test_sync: could not open semaphore\n");
                        return -1;
                }
        }

        uint64_t total  = 2 * (uint64_t)pairs;
        uint64_t pids[total];
        char *argvDec[] = {argv[1], "-1", argv[2], NULL};
        char *argvInc[] = {argv[1], "1", argv[2], NULL};

        global = 0;

        for (uint64_t i = 0; i < (uint64_t)pairs; i++) {
                /* Foreground so Ctrl+C kills the workers too. Otherwise they
                 * survive as orphans and keep corrupting `global` on later runs.
                 */
                int64_t dec = my_spawn("my_process_inc", 3, argvDec, 1, NO_PIPE,
                                       NO_PIPE);
                int64_t inc = my_spawn("my_process_inc", 3, argvInc, 1, NO_PIPE,
                                       NO_PIPE);
                if (dec < 0 || inc < 0) {
                        printf("test_sync: could not create process\n");
                        if (dec >= 0)
                                my_kill(dec);
                        if (inc >= 0)
                                my_kill(inc);
                        for (uint64_t j = 0; j < 2 * i; j++)
                                my_kill(pids[j]);
                        return -1;
                }
                pids[2 * i]     = dec;
                pids[2 * i + 1] = inc;
        }

        for (uint64_t i = 0; i < total; i++)
                my_wait(pids[i]);

        printf("Final value: %d\n", global);

        if (use_sem)
                my_sem_close(SEM_ID); /* release the launcher's reference */
        return 0;
}
