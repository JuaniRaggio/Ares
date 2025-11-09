; syscall.asm - Stub para ejecutar una syscall desde userland
global _syscall3

section .text
_syscall3:
    ; RDI = número de syscall
    ; RSI = arg1
    ; RDX = arg2
    ; RCX = arg3
    mov rax, rdi      ; número
    mov rdi, rsi      ; arg1
    mov rsi, rdx      ; arg2
    mov rdx, rcx      ; arg3
    syscall
    ret

_syscall0:
    mov rax, rdi
    syscall
    ret
