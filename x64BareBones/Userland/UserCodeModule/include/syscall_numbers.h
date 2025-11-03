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

// Cuando agregues una nueva syscall, incrementa este número
#define SYS_MAX 2

#endif /* SYSCALL_NUMBERS_H */
