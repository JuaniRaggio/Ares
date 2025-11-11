#include <benchmarks.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>

#define NO_DATA 0

fps_data fps_benchmark(uint8_t tests) {
        uint64_t start_ms, current_ms, inner_start_ms, frames = 0;
        fps_data collected_data = {
            .total_tests = tests,
            .max_fps     = 0,
            .min_fps     = -1,
        };
        syscall_get_time_ms(&start_ms);
        current_ms = start_ms;
        for (uint8_t frames_second = 0; tests > 0; --tests) {
                for (inner_start_ms = current_ms, frames_second = 0;
                     inner_start_ms + 1000 > current_ms;
                     ++frames_second, ++frames) {
                        syscall_redraw_screen();
                        syscall_get_time_ms(&current_ms);
                }
                maximize(&collected_data.max_fps, frames_second);
                minimize(&collected_data.min_fps, frames_second);
        }
        collected_data.sample_count       = frames;
        collected_data.total_test_time_ms = start_ms - current_ms;
        collected_data.average_fps =
            ((double)frames) / collected_data.total_test_time_ms;
        return collected_data;
}

void show_fps_benchmark(fps_data data) {
        printf("\n===== INIT FPS Benchmark Test =====\n");
        printf("Total test duration: %llu", data.total_test_time_ms);
        printf("Total tests produced: %llu", data.total_tests);
        printf("Total frames measured: %llu", data.sample_count);
        printf("Minimum FPS observed: %llu", data.min_fps);
        printf("Maximum FPS observed: %llu", data.max_fps);
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
