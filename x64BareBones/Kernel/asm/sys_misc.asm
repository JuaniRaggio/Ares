section .text
global sys_get_resolution_wrapper
global sys_get_register_array_wrapper
global sys_set_font_size_wrapper
global sys_get_memory_wrapper
global sys_draw_rect_wrapper

extern sys_get_resolution
extern sys_get_register_array
extern sys_set_font_size
extern sys_get_memory
extern sys_draw_rect

sys_get_resolution_wrapper:
    push rbp
    mov rbp, rsp
    call sys_get_resolution
    mov rsp, rbp
    pop rbp
    ret

sys_get_register_array_wrapper:
    push rbp
    mov rbp, rsp
    call sys_get_register_array
    mov rsp, rbp
    pop rbp
    ret

sys_set_font_size_wrapper:
    push rbp
    mov rbp, rsp
    call sys_set_font_size
    mov rsp, rbp
    pop rbp
    ret

sys_get_memory_wrapper:
    push rbp
    mov rbp, rsp
    call sys_get_memory
    mov rsp, rbp
    pop rbp
    ret

sys_draw_rect_wrapper:
    push rbp
    mov rbp, rsp
    call sys_draw_rect
    mov rsp, rbp
    pop rbp
    ret
