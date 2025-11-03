section .text
global sys_clear_wrapper
extern sys_clear

sys_clear_wrapper:
    push rbp
    mov rbp, rsp

    call sys_clear

    mov rsp, rbp
    pop rbp
    ret
