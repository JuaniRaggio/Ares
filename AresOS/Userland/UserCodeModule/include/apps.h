#pragma once

/**
 * @file apps.h
 * @brief Userland applications required by the assignment.
 *
 * Every application follows the standard process entry convention
 * main(argc, argv) and runs as a separate process created by the shell.
 */

#include <stdint.h>

typedef uint64_t (*app_main_t)(uint64_t argc, char *argv[]);

typedef struct {
        const char *name;
        const char *description;
        app_main_t main;
} app_t;

/** @brief Table of user-facing applications (for help and registration). */
extern const app_t app_registry[];
extern const int app_registry_count;

/**
 * @brief Register every application (and internal helpers) by name so the
 *        shell can spawn them as processes.
 */
void register_apps(void);

/** @brief Prints the memory manager state (total, used, free, etc). */
uint64_t mem_app(uint64_t argc, char *argv[]);

/** @brief Prints every process with pid, name, priority, stack and state. */
uint64_t ps_app(uint64_t argc, char *argv[]);

/** @brief Prints its pid with a greeting every N seconds (active wait). */
uint64_t loop_app(uint64_t argc, char *argv[]);

/** @brief Kills the process with the given pid. */
uint64_t kill_app(uint64_t argc, char *argv[]);

/** @brief Changes the priority of the process with the given pid. */
uint64_t nice_app(uint64_t argc, char *argv[]);

/** @brief Toggles a process between blocked and ready given its pid. */
uint64_t block_app(uint64_t argc, char *argv[]);

/** @brief Prints stdin as it is received. */
uint64_t cat_app(uint64_t argc, char *argv[]);

/** @brief Counts the number of lines in the input. */
uint64_t wc_app(uint64_t argc, char *argv[]);

/** @brief Filters the vowels out of the input. */
uint64_t filter_app(uint64_t argc, char *argv[]);

/** @brief Multiple readers / writers over a global variable (Haskell MVar). */
uint64_t mvar_app(uint64_t argc, char *argv[]);

/** @brief Integer division of two numbers: div <num> <divisor>. */
uint64_t div_app(uint64_t argc, char *argv[]);

/** @brief Triggers an invalid opcode exception (process-isolated demo). */
uint64_t opcode_app(uint64_t argc, char *argv[]);

/** @brief Plays the Tron game (WASD vs IJKL). */
uint64_t tron_app(uint64_t argc, char *argv[]);

/** @brief Memory dump of 32 bytes from an address: printmem <hex>. */
uint64_t printmem_app(uint64_t argc, char *argv[]);

/** @brief Runs performance benchmarks. */
uint64_t benchmark_app(uint64_t argc, char *argv[]);

/** @brief Lists all available commands. */
uint64_t help_app(uint64_t argc, char *argv[]);

/** @brief Shows the manual for a command: man <command>. */
uint64_t man_app(uint64_t argc, char *argv[]);

/** @brief Shows system and elapsed time. */
uint64_t time_app(uint64_t argc, char *argv[]);

/** @brief Clears the screen. */
uint64_t clear_app(uint64_t argc, char *argv[]);

/** @brief Shows captured CPU registers (capture with Ctrl+R). */
uint64_t inforeg_app(uint64_t argc, char *argv[]);

/** @brief Shows command history. */
uint64_t history_app(uint64_t argc, char *argv[]);

/** @brief Informs that the shell cannot exit. */
uint64_t exit_app(uint64_t argc, char *argv[]);

/** @brief Changes cursor shape: cursor <block|hollow|line|underline>. */
uint64_t cursor_app(uint64_t argc, char *argv[]);

/** @brief Changes text color: textcolor <color>. */
uint64_t textcolor_app(uint64_t argc, char *argv[]);

/** @brief Changes background color: bgcolor <color>. */
uint64_t bgcolor_app(uint64_t argc, char *argv[]);
