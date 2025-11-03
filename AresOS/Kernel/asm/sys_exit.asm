section .text
global sys_exit

; exit code in rdi
sys_exit:
    mov rax, rdi
    cli                 ; disable interrupts
.hang:
    hlt                 ; stop CPU
    jmp .hang

