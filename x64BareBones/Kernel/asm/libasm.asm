GLOBAL cpuVendor
; GLOBAL num2str
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

; ;; función que recibe un número positivo y una zona_de_memoria, y
; ;; transforma el número en un string terminado con cero
; ;; - IN: numero (positivo), zona_de_memoria - parámetros de entrada
; ;; - OUT: void - modifica zona_de_memoria, deja length en edx
; ;; - STACK: numero(ebp+8), zona_de_memoria(ebp+12)
; ;; - REG: eax=numero, edi=puntero_memoria, ebx=divisor, ecx=contador
; num2str:
;     ;; Armado de stack frame
;     push  rbp            ;; Backup del base pointer
;     mov   rbp, rsp        ;; Muevo al base pointer a la base del stack de esta funcion
;     mov   rax, [rbp + 16]  ;; Me guardo una copia del numero
;     mov   rdi, [rbp + 20]
;     mov   rbx, 10         ;; Divisor
;     mov   rcx, 0          ;; Contador
;
;
; zero_special:
;     cmp   rax, 0
;     jne   positive
;     mov   byte [rdi], '0'
;     mov   byte [rdi + 1], 0
;     jmp   end
;
;
; positive:
;     xor   rdx, rdx        ;; Para evitar restos de anteriores operaciones
;     div   rbx             ;; Div toma a eax como numerador y guarda el resto en edx
;     add   dl, '0'         ;; Le sumo al numero para que represente el ascii
;     push  dx             ;; Empujo el caracter al stack
;     inc   rcx             ;; Aumento el contador
;     cmp   rax, 0          ;; Comparo el numero con 0
;     jg    positive         ;; Si el numero sigue siendo mayor a 0 despues de la division, sigo en el loop
;    
;
;     xor   rbx, rbx
; pop_data:
;     pop   ax                  ;; Levanto el numero que tengo que escribir (Me queda el char en AL)
;     mov   [rdi + rbx], al     ;; Copio el byte bajo que es el unico que contiene el numero
;     inc   rbx                 ;; Aumento el iterador
;     cmp   rcx, rbx            ;; Comparo el iterador con la cantidad de digitos que tiene el numero
;     jg    pop_data             ;; Si ecx (cantidad de digitos) es mayor ebx (iterador), sigo en el loop
;     mov   byte [rdi + rbx], 0 ;; Agregar el cero final
;     inc   rbx
;     mov   rdx, rbx
;
; end:
;     ;; Desarmado de stack frame
;     mov   rsp, rbp
;     pop   rbp
;     ret

; keyboard_pool:
;     out 60h

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

