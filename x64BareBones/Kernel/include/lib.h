#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <sys/wait.h>

typedef struct {
        uint8_t hours;
        uint8_t minutes;
} s_time;

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);

char *cpuVendor(char *result);

uint8_t get_current_hour();
uint8_t get_current_minutes();
s_time get_current_time();

#endif
