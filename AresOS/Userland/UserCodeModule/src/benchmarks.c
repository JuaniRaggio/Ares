#include <benchmarks.h>
#include <lib.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>

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
        printf("Running FPS benchmark with %d test(s)...\n", tests);

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
        printf("\n===== INIT FPS Benchmark Test =====\n");
        printf("Total test duration: %llu\n", data.total_test_time_ms);
        printf("Total tests produced: %llu\n", data.total_tests);
        printf("Total frames measured: %llu\n", data.sample_count);
        printf("Minimum FPS observed: %llu\n", data.min_fps);
        printf("Maximum FPS observed: %llu\n", data.max_fps);
        printf("\n===== END  FPS Benchmark Test =====\n");
}

timer_data timer_benchmark(uint32_t duration_ms) {
}

void show_timer_benchmark(timer_data data) {
}

keyboard_data keyboard_benchmark(uint32_t num_keys) {
}

void show_keyboard_benchmark(keyboard_data data) {
}
