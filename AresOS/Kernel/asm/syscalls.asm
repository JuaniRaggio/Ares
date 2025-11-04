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
    swapgs                      ; cambiar a datos del kernel
    mov r15, rsp                ; guardar user RSP temporalmente en r15
    mov rsp, kernel_stack_top   ; cambiar a kernel stack

    ; Armar el stack frame
    push r15                    ; guardar user RSP
    push rcx                    ; guardar user RIP (dirección de retorno)
    push r11                    ; guardar user RFLAGS

    push rbx
    push rbp
    push r12
    push r13
    push r14

    ; syscall number está en RAX
    mov rbx, rax

    ; Buscar la función en la tabla de syscalls
    lea rcx, [rel syscalls_table]
    shl rbx, 3                  ; rbx * 8 (cada puntero es 8 bytes)
    add rcx, rbx

    mov rbx, [rcx]
    test rbx, rbx
    jz .unknown_syscall

    ; Los argumentos ya están en RDI, RSI, RDX correctamente
    ; Llamar a la función de syscall
    call rbx
    jmp .done

.unknown_syscall:
    mov rax, -1                 ; retornar error
    jmp .done

.done:
    ; Restaurar registros en orden inverso
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop r11                     ; restaurar user RFLAGS
    pop rcx                     ; restaurar user RIP
    pop rsp                     ; restaurar user RSP

    swapgs
    sysretq                     ; retornar a user mode
