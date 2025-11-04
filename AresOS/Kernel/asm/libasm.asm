GLOBAL cpuVendor
GLOBAL get_current_minutes
GLOBAL get_current_hour
GLOBAL get_input
GLOBAL _load_idt_register

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

get_current_minutes:
    xor   rax, rax
    mov   al, 0x0B
    out   70h, al           ; Seleccion el registro 0Bh para escribir
    mov   al, 4             ; Muevo lo que quiero escribir a un registro tmp
    out   71h, al           ; Escribo en el registro previamente seleccionado

    mov   al, 0x02          ; Para obtener los minutos
    out   70h, al           ; Selecciono el registro 04 (para obtener la hora)
    in    al, 71h           ; Leo lo que se me escribio
    movzx rax, al           ; Lo copio en el registro de convencion para return
    ret

get_current_hour:
    xor   rax, rax
    mov   al, 0x0B
    out   70h, al           ; Seleccion el registro 0Bh para escribir
    mov   al, 4             ; Muevo lo que quiero escribir a un registro tmp
    out   71h, al           ; Escribo en el registro previamente seleccionado

    mov   al, 0x04          ; Para obtener las horas
    out   70h, al           ; Selecciono el registro 04 (para obtener la hora)
    in    al, 71h           ; Leo lo que se me escribio
    movzx rax, al           ; Lo copio en el registro de convencion para return
    ret

get_input:
    xor rax, rax
    in al, 0x60
    ret

_load_idt_register:
    ; RDI points to IDTR structure (limit + base address)
    lidt [rdi]
    ret
