section .text
global sys_get_ticks_wrapper
global sys_get_seconds_wrapper
global sys_get_time_wrapper
global sys_get_rdtsc_wrapper
global sys_get_time_ms_wrapper
global sys_get_fps_wrapper
extern sys_get_ticks
extern sys_get_seconds
extern sys_get_time
extern sys_get_rdtsc
extern sys_get_time_ms
extern sys_get_fps

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

    call sys_get_time

    mov rsp, rbp
    pop rbp
    ret

sys_get_rdtsc_wrapper:
    push rbp
    mov rbp, rsp

    call sys_get_rdtsc

    mov rsp, rbp
    pop rbp
    ret

sys_get_time_ms_wrapper:
    push rbp
    mov rbp, rsp

    call sys_get_time_ms

    mov rsp, rbp
    pop rbp
    ret

sys_get_fps_wrapper:
    push rbp
    mov rbp, rsp

    call sys_get_fps

    mov rsp, rbp
    pop rbp
    ret
