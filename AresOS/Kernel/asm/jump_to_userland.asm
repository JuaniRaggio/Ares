; jump_to_userland.asm
; Salta a userland cambiando de Ring 0 a Ring 3

BITS 64
global jump_to_userland

section .bss
align 4096
user_stack: resb 4096 * 16      ; 16KB user stack
user_stack_top:

section .text

; void jump_to_userland(void *entry_point);
; RDI = entry point (0x400000)
jump_to_userland:
    ; Guardar entry point
    mov r15, rdi

    push 0x1B               ; SS (User Data Segment 0x18, RPL=3)
    lea rax, [rel user_stack_top]
    push rax                ; RSP (user stack pointer)

    ; Build RFLAGS with IF=1 (interrupts enabled)
    pushfq                  ; Get current RFLAGS
    pop rax
    or rax, 0x200           ; Set IF bit (bit 9)
    push rax                ; Push modified RFLAGS

    push 0x23               ; CS (User Code Segment 0x20, RPL=3)
    push r15                ; RIP (entry point = 0x400000)

    ; IRETQ will switch to ring 3 and restore RFLAGS with IF=1
    iretq
