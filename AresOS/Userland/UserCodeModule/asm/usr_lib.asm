global get_register_values
global get_current_minutes
global get_current_hour
global get_input

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
