global kernel_stack_top
section .bss
align 16
kernel_stack: resb 4096 * 4      ; 16 KiB
kernel_stack_top:
