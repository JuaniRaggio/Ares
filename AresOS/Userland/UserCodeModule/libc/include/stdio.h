#pragma once

#include <lib.h>
#include <stdarg.h> // Esta no deberia estar
#include <syscalls.h>

int printf(const char *format, ...);

int putchar(int c);

int puts(const char *s);

int getchar(void);

int scanf(char *fmt, ...);
