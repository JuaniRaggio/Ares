#include <stddef.h>
#include <apps.h>
#include <colors.h>
#include <mem_info.h>
#include <process_api.h>
#include <process_types.h>
#include <sem_api.h>
#include <stdio.h>
#include <syscalls.h>
#include <test_util.h>

#define MAX_SNAPSHOT 32
#define DEFAULT_LOOP_SECONDS 2
#define MVAR_EMPTY_SEM "mvar_empty"
#define MVAR_FULL_SEM "mvar_full"
#define MVAR_MAX_WRITERS 26
#define MVAR_WAIT_RANGE 50000000

static const char *const state_names[] = {
    "READY", "RUNNING", "BLOCKED", "DEAD", "ZOMBIE",
};

static const uint32_t reader_colors[] = {
    RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, GRAY, WHITE,
};
#define READER_COLORS_COUNT 8

/* Shared variable for mvar. All processes share the address space, so a
 * static is visible to every reader and writer. */
static char mvar_value;

static void print_padded(const char *s, int width) {
        int len = 0;
        while (s[len])
                len++;
        printf("%s", s);
        for (; len < width; len++)
                putchar(' ');
}

uint64_t mem_app(uint64_t argc, char *argv[]) {
        heap_stats_t stats;
        syscall_mem_stats(&stats);

        printf("Memory manager state:\n");
        printf("  Total:              %llu bytes\n",
               stats.total_heap_size_bytes);
        printf("  Used:               %llu bytes\n",
               stats.occupied_heap_space_bytes);
        printf("  Free:               %llu bytes\n",
               stats.available_heap_space_bytes);
        printf("  Largest free block: %llu bytes\n",
               stats.size_largest_free_block_bytes);
        printf("  Free blocks:        %llu\n", stats.number_of_free_blocks);
        printf("  Minimum ever free:  %llu bytes\n",
               stats.minimum_ever_free_bytes);
        printf("  Allocations:        %llu\n", stats.successful_allocations);
        printf("  Frees:              %llu\n", stats.successful_frees);
        return 0;
}

uint64_t ps_app(uint64_t argc, char *argv[]) {
        process_info_t info[MAX_SNAPSHOT];
        int count = syscall_ps(info, MAX_SNAPSHOT);

        printf("PID   NAME                PRIO  STATE    FG  RSP              "
               "STACK BASE\n");
        for (int i = 0; i < count; i++) {
                char pid_str[24];
                itoa(info[i].pid, pid_str, 10);
                print_padded(pid_str, 6);
                print_padded(info[i].name, 20);
                char prio_str[24];
                itoa(info[i].priority, prio_str, 10);
                print_padded(prio_str, 6);
                print_padded(state_names[info[i].state], 9);
                print_padded(info[i].foreground ? "*" : "-", 4);
                printf("%x %x\n", info[i].rsp, info[i].stack_base);
        }
        return 0;
}

/* Active wait on purpose: the assignment requires loop to wait without
 * blocking */
static void active_wait_seconds(uint64_t seconds) {
        uint64_t start = 0, now = 0;
        syscall_get_time_ms(&start);
        do {
                syscall_get_time_ms(&now);
        } while (now - start < seconds * 1000);
}

uint64_t loop_app(uint64_t argc, char *argv[]) {
        uint64_t seconds = DEFAULT_LOOP_SECONDS;
        if (argc >= 1 && satoi(argv[0]) > 0)
                seconds = satoi(argv[0]);

        int64_t pid = my_getpid();
        for (;;) {
                printf("Hello! I am process %d\n", (int)pid);
                active_wait_seconds(seconds);
        }
        return 0;
}

uint64_t kill_app(uint64_t argc, char *argv[]) {
        if (argc != 1) {
                printf("Usage: kill <pid>\n");
                return 1;
        }
        int64_t pid = satoi(argv[0]);
        if (my_kill(pid) == -1) {
                printf("kill: could not kill process %d\n", (int)pid);
                return 1;
        }
        /* Reap it: nobody else waits for an arbitrary killed process */
        my_wait(pid);
        printf("Process %d killed\n", (int)pid);
        return 0;
}

uint64_t nice_app(uint64_t argc, char *argv[]) {
        if (argc != 2) {
                printf("Usage: nice <pid> <priority>\n");
                return 1;
        }
        int64_t pid      = satoi(argv[0]);
        int64_t priority = satoi(argv[1]);
        if (my_nice(pid, priority) == -1) {
                printf("nice: could not change priority of process %d\n",
                       (int)pid);
                return 1;
        }
        printf("Process %d priority set to %d\n", (int)pid, (int)priority);
        return 0;
}

/* Returns the snapshot entry for pid, or NULL if the process is not alive */
static const process_info_t *find_process_info(const process_info_t *info,
                                               int count, int64_t pid) {
        for (int i = 0; i < count; i++) {
                if (info[i].pid == pid)
                        return &info[i];
        }
        return NULL;
}

/* Unblocks a blocked process or blocks a ready/running one */
static uint64_t toggle_block_state(int64_t pid, int currently_blocked) {
        if (currently_blocked) {
                if (my_unblock(pid) == -1) {
                        printf("block: could not unblock %d\n", (int)pid);
                        return 1;
                }
                printf("Process %d unblocked\n", (int)pid);
        } else {
                if (my_block(pid) == -1) {
                        printf("block: could not block %d\n", (int)pid);
                        return 1;
                }
                printf("Process %d blocked\n", (int)pid);
        }
        return 0;
}

uint64_t block_app(uint64_t argc, char *argv[]) {
        if (argc != 1) {
                printf("Usage: block <pid>\n");
                return 1;
        }
        int64_t pid = satoi(argv[0]);
        if (pid == 0) {
                printf("block: cannot block the shell\n");
                return 1;
        }

        process_info_t info[MAX_SNAPSHOT];
        int count = syscall_ps(info, MAX_SNAPSHOT);

        const process_info_t *proc = find_process_info(info, count, pid);
        if (proc == NULL) {
                printf("block: no process with pid %d\n", (int)pid);
                return 1;
        }
        return toggle_block_state(pid, proc->state == PROCESS_BLOCKED);
}

uint64_t cat_app(uint64_t argc, char *argv[]) {
        int c;
        while ((c = getchar()) != EOF)
                putchar(c);
        return 0;
}

uint64_t wc_app(uint64_t argc, char *argv[]) {
        int c;
        uint64_t lines = 0;
        while ((c = getchar()) != EOF) {
                if (c == '\n')
                        lines++;
        }
        printf("Lines: %llu\n", lines);
        return 0;
}

static int is_vowel(int c) {
        switch (c) {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
        case 'A':
        case 'E':
        case 'I':
        case 'O':
        case 'U':
                return 1;
        }
        return 0;
}

uint64_t filter_app(uint64_t argc, char *argv[]) {
        int c;
        while ((c = getchar()) != EOF) {
                if (!is_vowel(c))
                        putchar(c);
        }
        return 0;
}

static uint64_t mvar_writer(uint64_t argc, char *argv[]) {
        if (argc != 1)
                return 1;
        char letter = argv[0][0];

        for (;;) {
                bussy_wait(GetUniform(MVAR_WAIT_RANGE));
                my_sem_wait(MVAR_EMPTY_SEM);
                mvar_value = letter;
                my_sem_post(MVAR_FULL_SEM);
        }
        return 0;
}

static uint64_t mvar_reader(uint64_t argc, char *argv[]) {
        if (argc != 1)
                return 1;
        uint32_t color = reader_colors[satoi(argv[0]) % READER_COLORS_COUNT];

        for (;;) {
                bussy_wait(GetUniform(MVAR_WAIT_RANGE));
                my_sem_wait(MVAR_FULL_SEM);
                /* Print inside the critical section so the color of this
                 * reader cannot be clobbered by another reader. */
                syscall_set_text_color(color, STDOUT);
                putchar(mvar_value);
                syscall_set_text_color(WHITE, STDOUT);
                my_sem_post(MVAR_EMPTY_SEM);
        }
        return 0;
}

/* Resets the mvar semaphores in case a previous run was killed mid-flight */
static int reset_mvar_semaphores(void) {
        my_sem_close(MVAR_EMPTY_SEM);
        my_sem_close(MVAR_FULL_SEM);
        return my_sem_open(MVAR_EMPTY_SEM, 1) && my_sem_open(MVAR_FULL_SEM, 0);
}

/* Each writer gets a unique letter: A, B, C... */
static void spawn_mvar_writers(int64_t writers) {
        for (int64_t i = 0; i < writers; i++) {
                char letter[2] = {(char)('A' + i), 0};
                char *args[]   = {letter};
                my_create_process("mvar_writer", 1, args);
        }
}

/* Each reader gets an index that selects its output color */
static void spawn_mvar_readers(int64_t readers) {
        for (int64_t i = 0; i < readers; i++) {
                char index[24];
                itoa(i, index, 10);
                char *args[] = {index};
                my_create_process("mvar_reader", 1, args);
        }
}

uint64_t mvar_app(uint64_t argc, char *argv[]) {
        if (argc != 2) {
                printf("Usage: mvar <writers> <readers>\n");
                return 1;
        }
        int64_t writers = satoi(argv[0]);
        int64_t readers = satoi(argv[1]);
        if (writers <= 0 || readers <= 0 || writers > MVAR_MAX_WRITERS) {
                printf("mvar: writers and readers must be positive\n");
                return 1;
        }

        if (!reset_mvar_semaphores()) {
                printf("mvar: could not open semaphores\n");
                return 1;
        }

        spawn_mvar_writers(writers);
        spawn_mvar_readers(readers);

        /* The main process ends right after spawning readers and writers. */
        return 0;
}

const app_t app_registry[] = {
    {"mem", "Print the memory manager state", mem_app},
    {"ps", "List every process and its properties", ps_app},
    {"loop", "Greet with the pid every <seconds> (active wait)", loop_app},
    {"kill", "Kill a process: kill <pid>", kill_app},
    {"nice", "Change a process priority: nice <pid> <priority>", nice_app},
    {"block", "Toggle a process between blocked and ready: block <pid>",
     block_app},
    {"cat", "Print stdin as it is received", cat_app},
    {"wc", "Count the number of lines of the input", wc_app},
    {"filter", "Filter the vowels out of the input", filter_app},
    {"mvar", "Readers/writers over a global variable: mvar <w> <r>", mvar_app},
};

const int app_registry_count = sizeof(app_registry) / sizeof(app_registry[0]);

void register_apps(void) {
        for (int i = 0; i < app_registry_count; i++) {
                process_register(app_registry[i].name,
                                 (process_entry_t)app_registry[i].main);
        }
        /* Internal helpers spawned by mvar, not user-facing. */
        process_register("mvar_writer", (process_entry_t)mvar_writer);
        process_register("mvar_reader", (process_entry_t)mvar_reader);
}
