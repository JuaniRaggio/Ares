; syscalls.asm

BITS 64
section .text

global syscall_entry

extern syscall_dispatch
extern current_kernel_stack


; -------------------------------------------
; syscall_entry:
;  Entry point when user executes SYSCALL
;  Register convention (x86_64):
;    RAX = syscall number
;    RDI, RSI, RDX = arguments
;  Dispatch (table lookup + bounds check) is done in C by
;  syscall_dispatch(number, arg1, arg2, arg3).
; -------------------------------------------

syscall_entry:
    swapgs                      ; switch to kernel data
    mov r15, rsp                ; save user RSP temporarily in r15
    mov rsp, [rel current_kernel_stack]   ; per-process kernel stack

    ; Build stack frame
    push r15                    ; save user RSP
    push rcx                    ; save user RIP (return address)
    push r11                    ; save user RFLAGS

    push rbx
    push rbp
    push r12
    push r13
    push r14

    ; Shuffle into SysV order: dispatch(nr, a1, a2, a3)
    mov rcx, rdx                ; a3 (user RIP was already saved)
    mov rdx, rsi                ; a2
    mov rsi, rdi                ; a1
    mov rdi, rax                ; syscall number
    call syscall_dispatch

    ; Restore registers in reverse order
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop r11                     ; restore user RFLAGS
    pop rcx                     ; restore user RIP
    pop rsp                     ; restore user RSP

    swapgs
    o64 sysret
