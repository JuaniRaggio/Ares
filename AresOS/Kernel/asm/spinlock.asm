GLOBAL acquire_lock
GLOBAL release_lock

section .text
acquire_lock: // 

    mov rax, 1
    xchg rax, qword [rdi]
    cmp rax, #0
    jne acquire_lock
    ret

release_lock:

    mov qword [rdi], #0
    ret