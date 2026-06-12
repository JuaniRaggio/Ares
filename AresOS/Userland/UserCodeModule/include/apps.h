#pragma once

/**
 * @file apps.h
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

extern const app_t app_registry[];
extern const int app_registry_count;

void register_apps(void);
uint64_t mem_app(uint64_t argc, char *argv[]);
uint64_t ps_app(uint64_t argc, char *argv[]);
uint64_t loop_app(uint64_t argc, char *argv[]);
uint64_t kill_app(uint64_t argc, char *argv[]);
uint64_t nice_app(uint64_t argc, char *argv[]);
uint64_t block_app(uint64_t argc, char *argv[]);
uint64_t cat_app(uint64_t argc, char *argv[]);
uint64_t wc_app(uint64_t argc, char *argv[]);
uint64_t filter_app(uint64_t argc, char *argv[]);
uint64_t mvar_app(uint64_t argc, char *argv[]);
