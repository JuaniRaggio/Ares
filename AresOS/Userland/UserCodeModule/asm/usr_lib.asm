global get_register_values

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

