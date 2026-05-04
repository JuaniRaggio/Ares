section .text
global sys_malloc_wrapper
global sys_free_wrapper
global sys_mem_stats_wrapper

extern sys_malloc
extern sys_free
extern sys_mem_stats

sys_malloc_wrapper:
    push rbp
    mov rbp, rsp
    call sys_malloc
    mov rsp, rbp
    pop rbp
    ret

sys_free_wrapper:
    push rbp
    mov rbp, rsp
    call sys_free
    mov rsp, rbp
    pop rbp
    ret

sys_mem_stats_wrapper:
    push rbp
    mov rbp, rsp
    call sys_mem_stats
    mov rsp, rbp
    pop rbp
    ret
