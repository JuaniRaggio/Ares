section .text
global sys_write
extern ncPrintChar

; ==============================================
; RDI = file descriptor (1 = stdout, 2 = stderr)
; RSI = pointer to buffer  
; RDX = lenght
; ==============================================

sys_write:
    push rbp
    mov rbp, rsp

    cmp rdi, 1
    je .stdout
    cmp rdi, 2
    je .stderr
    jmp .done

.stdout:
    ; RDI = fd, RSI = buffer, RDX = length
    mov r13, rdx        ; r13 = counter (use callee-saved register)
    test r13, r13       ; if length == 0
    jz .done
    mov r12, rsi        ; r12 = buffer pointer
    push r12            ; preserve r12
    push r13            ; preserve r13
    push rbx            ; preserve rbx
    mov rbx, rdx        ; rbx = save original length

.loop_stdout:
    movzx edi, byte [r12]   ; load character from buffer
    mov sil, 0x0F           ; white color
    call ncPrintChar        ; ncPrintChar(char, color)

    inc r12                 ; next character
    dec r13                 ; decrement counter (r13 is preserved across calls)
    jnz .loop_stdout

    mov rax, rbx            ; return bytes written
    pop rbx
    pop r13
    pop r12
    jmp .end

.stderr:
    ; Similar but with red color
    mov r13, rdx        ; r13 = counter (use callee-saved register)
    test r13, r13       ; if length == 0
    jz .done
    mov r12, rsi        ; r12 = buffer pointer
    push r12            ; preserve r12
    push r13            ; preserve r13
    push rbx            ; preserve rbx
    mov rbx, rdx        ; rbx = save original length

.loop_stderr:
    movzx edi, byte [r12]   ; load character from buffer
    mov sil, 0x04           ; red color
    call ncPrintChar

    inc r12                 ; next character
    dec r13                 ; decrement counter
    jnz .loop_stderr

    mov rax, rbx            ; return bytes written
    pop rbx
    pop r13
    pop r12
    jmp .end

.done:
    mov rax, rdx        ; written bytes amount
.end:
    mov rsp, rbp
    pop rbp
    ret

