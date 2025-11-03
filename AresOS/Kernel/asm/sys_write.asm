section .text
global sys_write
extern ncPrint, ncPrintChar

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
    mov rcx, rdx        ; rcx = contador
    test rcx, rcx       ; si length == 0
    jz .done
    mov r12, rsi        ; r12 = puntero al buffer
    push r12            ; preservar r12
    push rbx            ; preservar rbx
    mov rbx, rcx        ; rbx = guardar length original

.loop_stdout:
    movzx edi, byte [r12]   ; cargar un caracter del buffer
    mov sil, 0x0F           ; color blanco
    call ncPrintChar        ; ncPrintChar(char, color)

    inc r12                 ; siguiente caracter
    dec rcx
    jnz .loop_stdout

    mov rax, rbx            ; retornar bytes escritos
    pop rbx
    pop r12
    jmp .end

.stderr:
    ; Similar pero con color rojo
    mov rcx, rdx
    test rcx, rcx
    jz .done
    mov r12, rsi
    push r12
    push rbx
    mov rbx, rcx

.loop_stderr:
    movzx edi, byte [r12]
    mov sil, 0x04           ; color rojo
    call ncPrintChar

    inc r12
    dec rcx
    jnz .loop_stderr

    mov rax, rbx
    pop rbx
    pop r12
    jmp .end

.done:
    mov rax, rdx        ; written bytes amount
.end:
    mov rsp, rbp
    pop rbp
    ret

