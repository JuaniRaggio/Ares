GLOBAL cpuVendor
GLOBAL get_current_minutes
GLOBAL get_current_hour

section .text
	
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

