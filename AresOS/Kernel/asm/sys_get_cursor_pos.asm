section .text
global sys_get_cursor_pos_wrapper
extern sys_get_cursor_pos

sys_get_cursor_pos_wrapper:
    push rbp
    mov rbp, rsp

    call sys_get_cursor_pos

    mov rsp, rbp
    pop rbp
    ret
