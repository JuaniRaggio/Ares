section .text
global sys_redraw_screen_wrapper
extern sys_redraw_screen

sys_redraw_screen_wrapper:
    push rbp
    mov rbp, rsp

    call sys_redraw_screen

    mov rsp, rbp
    pop rbp
    ret
