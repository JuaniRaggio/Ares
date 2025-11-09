; syscalls.asm

BITS 64
section .text

global syscall_entry

extern syscalls_table
extern kernel_stack_top
extern sys_write, sys_exit


; -------------------------------------------
; syscall_entry:
;  Entry point when user executes SYSCALL
;  Register convention (x86_64):
;    RAX = syscall number
; -------------------------------------------

syscall_entry:
    swapgs                      ; switch to kernel data
    mov r15, rsp                ; save user RSP temporarily in r15
    mov rsp, kernel_stack_top   ; switch to kernel stack

    ; Build stack frame
    push r15                    ; save user RSP
    push rcx                    ; save user RIP (return address)
    push r11                    ; save user RFLAGS

    push rbx
    push rbp
    push r12
    push r13
    push r14

    ; syscall number is in RAX
    mov rbx, rax

    ; Look up function in syscalls table
    lea rcx, [rel syscalls_table]
    shl rbx, 3                  ; rbx * 8 (each pointer is 8 bytes)
    add rcx, rbx

    mov rbx, [rcx]
    test rbx, rbx
    jz .unknown_syscall

    ; Arguments are already in RDI, RSI, RDX correctly
    ; Call syscall function
    call rbx
    jmp .done

.unknown_syscall:
    mov rax, -1                 ; return error
    jmp .done

.done:
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
    sysretq                     ; return to user mode
