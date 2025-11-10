BITS 64
global setup_tss
global tss64
global tss_size

extern kernel_stack_top

section .data
align 16

tss64:
    dd 0                ; Reserved
    dq kernel_stack_top ; RSP0
    dq 0                ; RSP1
    dq 0                ; RSP2
    dq 0                ; Reserved
    dq 0                ; IST1
    dq 0                ; IST2
    dq 0                ; IST3
    dq 0                ; IST4
    dq 0                ; IST5
    dq 0                ; IST6
    dq 0                ; IST7
    dq 0                ; Reserved
    dw 104              ; I/O Map Base Address, must be size of TSS or greater
    dw 0                ; Reserved

tss_size equ $ - tss64

section .text

setup_tss:
    mov ax, 0x28
    ltr ax
    ret
