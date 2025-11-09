; syscall.asm - Stub para ejecutar una syscall desde userland
global _syscall3
extern SYS_GET_REGISTER_ARRAY

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
    mov rax, SYS_GET_REGISTER_ARRAY
    syscall
    ret
