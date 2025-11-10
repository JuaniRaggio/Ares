GLOBAL cpuVendor
GLOBAL get_current_minutes
GLOBAL get_current_hour
GLOBAL get_current_seconds
GLOBAL get_input
GLOBAL _load_idt_register
GLOBAL read_tsc
GLOBAL outb
GLOBAL inb

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

_load_idt_register:
    ; RDI points to IDTR structure (limit + base address)
    lidt [rdi]
    ret

; Read Time Stamp Counter (TSC)
; Returns 64-bit cycle counter in RAX
read_tsc:
    rdtsc                ; EDX:EAX = TSC
    shl rdx, 32          ; Shift high 32 bits to upper half
    or rax, rdx          ; Combine into 64-bit value in RAX
    ret

; Write byte to I/O port
; RDI = port (uint16_t)
; RSI = value (uint8_t)
outb:
    mov dx, di           ; Port number to DX
    mov al, sil          ; Value to AL
    out dx, al           ; Write AL to port DX
    ret

; Read byte from I/O port
; RDI = port (uint16_t)
; Returns value in AL (zero-extended to RAX)
inb:
    mov dx, di           ; Port number to DX
    xor rax, rax         ; Clear RAX
    in al, dx            ; Read from port DX into AL
    ret
