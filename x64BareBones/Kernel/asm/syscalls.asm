; syscalls.asm

BITS 64
section .text

global syscall_entry

extern syscalls_table
extern kernel_stack_top
extern sys_write, sys_exit


; -------------------------------------------
; syscall_entry:
;  Punto de entrada cuando el usuario ejecuta SYSCALL
;  Convención de registros (x86_64):
;    RAX = número de syscall
; -------------------------------------------

syscall_entry:
    mov rsp, kernel_stack_top  ; use the kernel stack 
    swapgs                     
    push rdi                    
    push rsi
    push rdx
    push rcx
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; syscall number is in RAX
    mov rbx, rax

    lea rcx, [rel syscalls_table]

    shl rbx, 3                  ; rbx * 3
    add rcx, rbx

    mov rbx, [rcx]
    test rbx, rbx
    jz .unknown_syscall         

    call rbx
    jmp .done

.unknown_syscall:
    mov rax, -1                 ; not a valid syscall
    jmp .done

.done:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    swapgs
    sysretq                     ; return to user mode
