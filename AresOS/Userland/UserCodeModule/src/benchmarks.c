#include <benchmarks.h>
#include <lib.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>

#define for_ever for (;;)
#define NO_DATA 0

/* Simple pseudo-random number generator for colors */
static uint32_t rand_state = 12345;
static uint32_t simple_rand(void) {
        rand_state = rand_state * 1103515245 + 12345;
        return (rand_state / 65536) % 32768;
}

fps_data fps_benchmark(uint8_t tests) {
        uint64_t start_ms, current_ms, inner_start_ms, frames = 0;
        uint32_t screen_width, screen_height;

        fps_data collected_data = {
            .total_tests = tests,
            .max_fps     = 0,
            .min_fps     = UINT64_MAX,
        };

        syscall_get_resolution(&screen_width, &screen_height);

        syscall_clear();

        syscall_get_time_ms(&start_ms);
        current_ms = start_ms;

        for (uint8_t frames_second = 0; tests > 0; --tests) {
                for (inner_start_ms = current_ms, frames_second = 0;
                     current_ms < inner_start_ms + 1000;
                     ++frames_second, ++frames) {
                        uint32_t color = (simple_rand() % 0xFFFFFF);
                        uint16_t x     = simple_rand() % (screen_width - 100);
                        uint16_t y     = simple_rand() % (screen_height - 100);
                        syscall_draw_rect(x, y, 100, 100, color);
                        syscall_get_time_ms(&current_ms);
                }
                maximize(&collected_data.max_fps, frames_second);
                minimize(&collected_data.min_fps, frames_second);
        }
        collected_data.sample_count       = frames;
        collected_data.total_test_time_ms = current_ms - start_ms;
        collected_data.average_fps =
            (((double)frames * 1000.0) / collected_data.total_test_time_ms);

        /* Restore screen content from text buffer */
        syscall_redraw_screen();
        return collected_data;
}

void show_fps_benchmark(fps_data data) {
        printf("\n========= FPS Benchmark Result =========\n");
        printf("Total test duration: %llu ms\n", data.total_test_time_ms);
        printf("Total tests produced: %llu\n", data.total_tests);
        printf("Total frames measured: %llu\n", data.sample_count);
        printf("Minimum FPS observed: %llu\n", data.min_fps);
        printf("Maximum FPS observed: %llu\n", data.max_fps);
        printf("Average FPS observed: %llu\n", data.average_fps);
}

timer_data timer_benchmark(uint32_t duration_ms) {
        uint64_t start_ticks, current_ticks;
        uint64_t start_ms, current_ms;
        uint64_t last_tick_time_ms = 0;
        uint64_t max_jitter        = 0;

        timer_data collected_data = {
            .total_time_ms = duration_ms,
        };

        printf("Running timer benchmark for %lu ms...\n", duration_ms);

        /* Get initial measurements */
        syscall_get_ticks(&start_ticks);
        syscall_get_time_ms(&start_ms);
        last_tick_time_ms = start_ms;

        uint64_t end_ms = start_ms + duration_ms;

        /* Monitor timer ticks for the specified duration */
        uint64_t last_ticks = start_ticks;
        for_ever {
                syscall_get_time_ms(&current_ms);
                if (current_ms >= end_ms)
                        break;

                syscall_get_ticks(&current_ticks);

                /* Check if a new tick occurred */
                if (current_ticks != last_ticks) {
                        /* Calculate jitter: difference from expected tick time
                         */
                        uint64_t elapsed_since_last_tick =
                            current_ms - last_tick_time_ms;

                        /* Expected: ~55ms per tick (18.2 Hz) */
                        uint64_t expected_interval = 55;
                        uint64_t jitter =
                            (elapsed_since_last_tick > expected_interval)
                                ? (elapsed_since_last_tick - expected_interval)
                                : (expected_interval - elapsed_since_last_tick);

                        if (jitter > max_jitter) {
                                max_jitter = jitter;
                        }

                        last_tick_time_ms = current_ms;
                        last_ticks        = current_ticks;
                }
        }

        syscall_get_ticks(&current_ticks);
        syscall_get_time_ms(&current_ms);

        collected_data.actual_ticks  = current_ticks - start_ticks;
        collected_data.total_time_ms = current_ms - start_ms;
        collected_data.expected_ticks =
            (collected_data.total_time_ms * 18) / 1000;
        collected_data.missed_ticks =
            (collected_data.expected_ticks > collected_data.actual_ticks)
                ? (collected_data.expected_ticks - collected_data.actual_ticks)
                : 0;
        collected_data.max_jitter_us = max_jitter * 1000;

        /* Calculate accuracy percentage */
        if (collected_data.expected_ticks > 0) {
                collected_data.accuracy_percent =
                    (((double)collected_data.actual_ticks * 100.0) /
                     collected_data.expected_ticks);
        } else {
                collected_data.accuracy_percent = 0.0;
        }

        /* Restore screen */
        syscall_redraw_screen();

        return collected_data;
}

void show_timer_benchmark(timer_data data) {
        printf("\n======= Timer Hardware Benchmark =======\n");
        printf("Test duration: %llu ms\n", data.total_time_ms);
        printf("Expected ticks: %llu\n", data.expected_ticks);
        printf("Actual ticks: %llu\n", data.actual_ticks);
        printf("Missed ticks: %llu\n", data.missed_ticks);
        // printf("Timer accuracy: %.2f%%\n", data.accuracy_percent);
        printf("Max jitter: %llu us\n", data.max_jitter_us);
}

keyboard_data keyboard_benchmark(uint32_t num_keys) {
        uint64_t total_latency = 0;
        uint64_t min_latency   = UINT64_MAX;
        uint64_t max_latency   = 0;
        uint64_t key_count     = 0;

        keyboard_data collected_data = {
            .key_presses        = num_keys,
            .missed_events      = 0,
            .min_latency_us     = 0,
            .max_latency_us     = 0,
            .average_latency_us = 0,
        };

        printf("\n========== Keyboard Benchmark ==========\n");
        printf("Please press %lu keys...\n", num_keys);
        printf("(Press any keys to test keyboard latency)\n\n");

        /* Measure latency for each keypress */
        for (uint32_t i = 0; i < num_keys; i++) {
                uint64_t wait_start_ms, key_detected_ms;
                char c;

                /* Start timing - waiting for key */
                syscall_get_time_ms(&wait_start_ms);

                /* Poll for keypress */
                do {
                        c = getchar();
                } while (c == 0);

                /* Key detected - measure time */
                syscall_get_time_ms(&key_detected_ms);

                /* Calculate latency from start of polling to key detection */
                /* Note: This measures polling latency, not true hardware
                 * latency */
                uint64_t latency_ms = key_detected_ms - wait_start_ms;
                uint64_t latency_us = latency_ms * 1000;

                /* For a more accurate hardware latency, we'd need to measure
                 * from IRQ to userspace, but this gives us a good approximation
                 */
                if (latency_us < min_latency) {
                        min_latency = latency_us;
                }
                if (latency_us > max_latency) {
                        max_latency = latency_us;
                }

                total_latency += latency_us;
                key_count++;

                printf("Key %lu/%lu: '%c' - Latency: %llu us\n", i + 1,
                       num_keys, (c >= 32 && c <= 126) ? c : '?', latency_us);
        }

        /* Calculate statistics */
        collected_data.key_presses    = key_count;
        collected_data.min_latency_us = min_latency;
        collected_data.max_latency_us = max_latency;
        collected_data.average_latency_us =
            (key_count > 0) ? (total_latency / key_count) : 0;
        collected_data.missed_events =
            0; /* Not tracking this in current impl */

        /* Restore screen */
        syscall_redraw_screen();

        return collected_data;
}

void show_keyboard_benchmark(keyboard_data data) {
        printf("\n========== Keyboard Benchmark ==========\n");
        printf("Total keys pressed: %llu\n", data.key_presses);
        printf("Minimum latency: %llu us\n", data.min_latency_us);
        printf("Maximum latency: %llu us\n", data.max_latency_us);
        printf("Average latency: %llu us\n", data.average_latency_us);
        printf("Missed events: %llu\n", data.missed_events);
}
