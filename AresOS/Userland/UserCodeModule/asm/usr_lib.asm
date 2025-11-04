global get_register_values
global get_current_seconds
global get_current_minutes
global get_current_hour

struc regs
        _rip: resq 1
        _rsp: resq 1
        _rax: resq 1
        _rbx: resq 1
        _rcx: resq 1
        _rdx: resq 1
        _rbp: resq 1
        _rdi: resq 1
        _rsi: resq 1
        _r8:  resq 1
        _r9:  resq 1
        _r10: resq 1
        _r11: resq 1
        _r12: resq 1
        _r13: resq 1
        _r14: resq 1
        _r15: resq 1
endstruc

.bss
register_snapshot: resb regs_size

.text

; @param: void
; @return: struc regs with all register values
get_register_values:
    mov qword [register_snapshot + _rax], rax
    mov qword [register_snapshot + _rbx], rbx
    mov qword [register_snapshot + _rcx], rcx
    mov qword [register_snapshot + _rdx], rdx
    mov qword [register_snapshot + _rsi], rsi
    mov qword [register_snapshot + _rdi], rdi
    mov qword [register_snapshot + _rbp], rbp
    mov qword [register_snapshot + _rsp], rsp
    mov qword [register_snapshot + _r8],  r8
    mov qword [register_snapshot + _r9],  r9
    mov qword [register_snapshot + _r10], r10
    mov qword [register_snapshot + _r11], r11
    mov qword [register_snapshot + _r12], r12
    mov qword [register_snapshot + _r13], r13
    mov qword [register_snapshot + _r14], r14
    mov qword [register_snapshot + _r15], r15

    lea rax, [register_snapshot]
    ret

