section .text
global sys_set_text_color_wrapper
extern sys_set_text_color

sys_set_text_color_wrapper:
    push rbp
    mov rbp, rsp

    call sys_set_text_color

    mov rsp, rbp
    pop rbp
    ret
