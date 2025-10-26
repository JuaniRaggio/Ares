// lib.h

#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <sys/wait.h> // lo dejás como estaba

typedef struct {
        uint8_t hours;
        uint8_t minutes;
} s_time;

// Memoria
void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);

// CPU info
char *cpuVendor(char *result);

// Tiempo
uint8_t get_current_hour(void);
uint8_t get_current_minutes(void);
s_time get_current_time(void);

// Funciones de string
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, uint64_t n);
uint64_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, uint64_t n);

#endif
