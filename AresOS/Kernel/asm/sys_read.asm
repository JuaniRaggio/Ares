section .text
global sys_read_wrapper
extern sys_read

; ==============================================
; RDI = file descriptor (0 = stdin)
; RSI = pointer to buffer
; RDX = count
; ==============================================

sys_read_wrapper:
    push rbp
    mov rbp, rsp

    ; Los argumentos ya están en los registros correctos (RDI, RSI, RDX)
    call sys_read

    mov rsp, rbp
    pop rbp
    ret
