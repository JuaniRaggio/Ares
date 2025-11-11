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

void fps_benchmark(void);

void show_fps_benchmark(void);

void timer_benchmark(void);

void show_timer_benchmark(void);

void keyboard_benchmark(void);

void show_keyboard_benchmark(void);
