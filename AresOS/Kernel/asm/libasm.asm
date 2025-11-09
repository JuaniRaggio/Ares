GLOBAL cpuVendor
GLOBAL get_current_minutes
GLOBAL get_current_hour
GLOBAL get_current_seconds
GLOBAL get_input
GLOBAL _load_idt_register
GLOBAL get_register_snapshot

struc regs
        _r15: resq 1
        _r14: resq 1
        _r13: resq 1
        _r12: resq 1
        _r11: resq 1
        _r10: resq 1
        _r9:  resq 1
        _r8:  resq 1
        _rsi: resq 1
        _rdi: resq 1
        _rbp: resq 1
        _rdx: resq 1
        _rcx: resq 1
        _rbx: resq 1
        _rax: resq 1
        _rip: resq 1
        _cs:  resq 1
        _rflags: resq 1
        _rsp: resq 1
        _ss:  resq 1
endstruc

section .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

; Convert BCD to binary
; Input: AL = BCD value
; Output: AL = binary value
; Formula: binary = (BCD >> 4) * 10 + (BCD & 0x0F)
bcd_to_binary:
    push rcx
    push rdx

    mov cl, al           ; Save original BCD value
    and al, 0x0F         ; AL = lower nibble (ones digit)

    shr cl, 4            ; CL = upper nibble (tens digit)
    mov dl, cl           ; DL = tens
    shl dl, 3            ; DL = tens * 8
    shl cl, 1            ; CL = tens * 2
    add dl, cl           ; DL = tens * 10

    add al, dl           ; AL = tens*10 + ones

    pop rdx
    pop rcx
    ret

get_current_seconds:
    xor   rax, rax

    ; Disable NMI and select seconds register
    mov   al, 0x00
    out   70h, al
    in    al, 71h        ; Read seconds

    ; Convert from BCD to binary
    call bcd_to_binary

    movzx rax, al
    ret

get_current_minutes:
    xor   rax, rax

    ; Disable NMI and select minutes register
    mov   al, 0x02
    out   70h, al
    in    al, 71h        ; Read minutes

    ; Convert from BCD to binary
    call bcd_to_binary

    movzx rax, al
    ret

get_current_hour:
    xor   rax, rax

    ; Disable NMI and select hours register
    mov   al, 0x04
    out   70h, al
    in    al, 71h        ; Read hours

    ; Assume 24-hour format, just convert from BCD
    call  bcd_to_binary

    movzx rax, al
    ret

get_input:
    xor rax, rax
    in al, 0x60
    ret

get_register_snapshot:
    mov qword [regs_snapshot + _r15], r15
    mov qword [regs_snapshot + _r14], r14
    mov qword [regs_snapshot + _r13], r13
    mov qword [regs_snapshot + _r12], r12
    mov qword [regs_snapshot + _r11], r11
    mov qword [regs_snapshot + _r10], r10
    mov qword [regs_snapshot + _r9], r9
    mov qword [regs_snapshot + _r8], r8
    mov qword [regs_snapshot + _rsi], rsi
    mov qword [regs_snapshot + _rdi], rsi
    mov qword [regs_snapshot + _rbp], rbp
    mov qword [regs_snapshot + _rdx], rdx
    mov qword [regs_snapshot + _rcx], rcx
    mov qword [regs_snapshot + _rbx], rbx
    mov qword [regs_snapshot + _rax], rax
    mov qword [regs_snapshot + _rip], rip
    mov qword [regs_snapshot + _cs], cs
    mov qword [regs_snapshot + _rflags], rflags
    mov qword [regs_snapshot + _rsp], rsp
    mov qword [regs_snapshot + _ss], ss
    ret

_load_idt_register:
    ; RDI points to IDTR structure (limit + base address)
    lidt [rdi]
    ret

section .bss
    regs_snapshot resq 20
