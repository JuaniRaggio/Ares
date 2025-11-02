// unistd.h - Unix Standard Definitions

#ifndef UNISTD_H
#define UNISTD_H

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// IO
long write(int fd, const void *buf, unsigned long count);
long read(int fd, void *buf, unsigned long count);

void exit(int status) __attribute__((noreturn));

#endif
