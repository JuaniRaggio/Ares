; sys_exit.asm

global sys_exit

; exit code in rdi

sys_exit:
    mov rax, rdi
    cli
.hang:
    hlt                 ; stops the CPU
    jmp .hang

