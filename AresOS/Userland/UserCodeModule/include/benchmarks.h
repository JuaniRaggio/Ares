#pragma once

#include <stdint.h>

typedef struct {
        uint64_t min;
        uint64_t max;
        uint64_t average;
} fps_data;

typedef struct {
} keyboard_data;

typedef struct {
        uint64_t ticks;
        uint64_t time;
} timer_data;

/** FPS testing/benchmarking function
 */
fps_data fps_benchmark(void);

/**
 */
void show_fps_benchmark(fps_data data);

/**
 */
timer_data timer_benchmark(void);

/**
 */
void show_timer_benchmark(timer_data data);

/**
 */
keyboard_data keyboard_benchmark(void);

/**
 */
void show_keyboard_benchmark(keyboard_data data);
