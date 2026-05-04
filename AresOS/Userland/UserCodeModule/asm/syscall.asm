; syscall.asm - Stub to execute a syscall from userland
global _syscall3
global _process_exit_stub

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

; Called when a process function returns.
; RAX holds the function's return value, used as exit code.
_process_exit_stub:
    mov rdi, rax      ; exit code = return value of the process function
    mov rax, 1        ; SYS_EXIT = 1
    syscall
    ; should never reach here
    jmp _process_exit_stub
