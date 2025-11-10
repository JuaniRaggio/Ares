BITS 64
global setup_tss
global tss64

extern kernel_stack_top

section .data
align 16

tss64:
    dd 0
    dq kernel_stack_top
    times 11 dq 0
    dd 0
    dd 0
    dw 0
    dw 0

tss_size equ $ - tss64

section .text

setup_tss:
    mov ax, 0x28
    ltr ax
    ret
