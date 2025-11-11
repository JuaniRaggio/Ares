#pragma once
#include <stdint.h>

typedef struct {
        uint64_t sample_count;       // Number of frames measured
        uint64_t total_test_time_ms; // Total test duration
        uint64_t total_tests;        // Total tests produced
        uint64_t min_fps;            // Minimum FPS observed
        uint64_t max_fps;            // Maximum FPS observed
        uint64_t average_fps;        // Average FPS
} fps_data;

typedef struct {
        uint64_t key_presses;        // Number of keys pressed in test
        uint64_t min_latency_us;     // Minimum latency (microseconds)
        uint64_t max_latency_us;     // Maximum latency
        uint64_t average_latency_us; // Average latency
        uint64_t missed_events;      // Missed events (if applicable)
} keyboard_data;

typedef struct {
        uint64_t expected_ticks; // Expected ticks
        uint64_t actual_ticks;   // Actual ticks received
        uint64_t missed_ticks;   // Missed ticks
        uint64_t total_time_ms;  // Total test time (ms)
        // Max jitter (diference between expected ticks and actual ticks)
        uint64_t max_jitter_us;
        double accuracy_percent; // Timer accuracy (%)
} timer_data;

/**
 * Executes a benchmark for FPS during duration_ms time
 * @param tests amount of tests desired
 * @return FPS statistics for this specific task
 */
fps_data fps_benchmark(uint8_t tests);

/**
 * Show benchmark results
 */
void show_fps_benchmark(fps_data data);

/**
 * Executes a timer benchmark for a specified period
 * @param duration_ms Test duration in milliseconds
 * @return Timer statistics
 */
timer_data timer_benchmark(uint32_t duration_ms);

/**
 * Shows timer benchmark results
 */
void show_timer_benchmark(timer_data data);

/**
 * Executes a keyboard benchmark
 * @param num_keys Number of keyboards desired to test
 * @return Keyboard statistics
 */
keyboard_data keyboard_benchmark(uint32_t num_keys);

/**
 * Shows keyboard benchmark results
 */
void show_keyboard_benchmark(keyboard_data data);
