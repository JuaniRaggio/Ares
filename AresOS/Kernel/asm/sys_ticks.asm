section .text
global sys_get_ticks_wrapper
global sys_get_seconds_wrapper
global sys_get_time_wrapper
extern sys_get_ticks
extern sys_get_seconds
extern sys_get_time

sys_get_ticks_wrapper:
    push rbp
    mov rbp, rsp

    call sys_get_ticks

    mov rsp, rbp
    pop rbp
    ret

sys_get_seconds_wrapper:
    push rbp
    mov rbp, rsp

    call sys_get_seconds

    mov rsp, rbp
    pop rbp
    ret

sys_get_time_wrapper:
    push rbp
    mov rbp, rsp

    ; RDI already contains the pointer to s_time
    call sys_get_time

    mov rsp, rbp
    pop rbp
    ret
