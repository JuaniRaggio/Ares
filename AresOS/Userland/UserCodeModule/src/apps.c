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

