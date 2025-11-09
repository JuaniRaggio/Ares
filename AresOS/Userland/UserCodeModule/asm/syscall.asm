; syscall.asm - Stub to execute a syscall from userland
global _syscall3
extern SYS_GET_REGISTER_ARRAY

section .text
_syscall3:
    ; RDI = syscall number
    ; RSI = arg1
    ; RDX = arg2
    ; RCX = arg3
    mov rax, rdi      ; number
    mov rdi, rsi      ; arg1
    mov rsi, rdx      ; arg2
    mov rdx, rcx      ; arg3
    syscall
    ret
