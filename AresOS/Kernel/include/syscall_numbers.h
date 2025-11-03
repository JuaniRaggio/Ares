/* syscall_numbers.h - Números de syscalls
 * CONTRATO entre kernel y userland
 * Este archivo es compartido por ambos
 */
#ifndef SYSCALL_NUMBERS_H
#define SYSCALL_NUMBERS_H

// Números de syscalls - deben coincidir en kernel y userland
#define SYS_WRITE 0
#define SYS_EXIT 1
#define SYS_READ 2
#define SYS_CLEAR 3
#define SYS_GET_TICKS 4
#define SYS_GET_RESOLUTION 5
#define SYS_GET_REGISTER_ARRAY 6
#define SYS_SET_FONT_SIZE 7
#define SYS_GET_MEMORY 8
#define SYS_DRAW_RECT 9
#define SYS_GET_SECONDS 10

// Cuando agregues una nueva syscall, incrementa este número
#define SYS_MAX 10

#endif /* SYSCALL_NUMBERS_H */
