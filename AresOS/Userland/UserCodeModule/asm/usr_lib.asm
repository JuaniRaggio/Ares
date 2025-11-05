global get_register_values

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
        _cs:  resq 1
        _rflags: resq 1
        _ss:  resq 1
endstruc

.bss
register_snapshot: resb regs_size

.text

; @param: void
; @return: struc regs with all register values from last interrupt/exception
; Note: This function calls the kernel to get the saved register state
extern sys_get_register_array

get_register_values:
    push rbp
    mov rbp, rsp

    ; Call kernel syscall to get saved registers
    lea rdi, [register_snapshot]
    call sys_get_register_array

    ; Return pointer to snapshot
    lea rax, [register_snapshot]

    mov rsp, rbp
    pop rbp
    ret

