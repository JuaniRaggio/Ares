section .text
global sys_set_bg_color_wrapper
extern sys_set_bg_color

sys_set_bg_color_wrapper:
    push rbp
    mov rbp, rsp

    call sys_set_bg_color

    mov rsp, rbp
    pop rbp
    ret
