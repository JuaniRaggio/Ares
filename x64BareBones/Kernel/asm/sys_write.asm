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
    mov rdi, rsi        
    mov sil, 0x0F       ; white 
    call ncPrint
    jmp .done

.stderr:
    mov rdi, rsi
    mov sil, 0x04       ; red
    call ncPrint
    jmp .done

.done:
    mov rax, rdx        ; written bytes amount
    mov rsp, rbp
    pop rbp
    ret

; =========================
; some other useful colors:
; =========================
; 0x00  Black             |
; 0x01	Blue	            |
; 0x02	Green	            |
; 0x04	Red               |	
; 0x07	Grey              |
; 0x0F	White             |
; =========================

