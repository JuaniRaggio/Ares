;interrupts.asm
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler


GLOBAL _exception0Handler
GLOBAL _exception6Handler

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN getStackBase

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
        _cs:  resq 1
        _rflags: resq 1
        _ss:  resq 1
endstruc

SECTION .text

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

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	mov rsi, rsp ; pasar el stack pointer para captura de registros
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro



%macro exceptionHandler 1
        cli
        pushState

        ; guardamos los valores de los registros generales
        mov [regs + _rax], rax
        mov [regs + _rbx], rbx      
        mov [regs + _rcx], rcx  
        mov [regs + _rdx], rdx      
        mov [regs + _rbp], rbp      
        mov [regs + _rdi], rdi      
        mov [regs + _rsi], rsi    
        mov [regs + _r8], r8
        mov [regs + _r9], r9
        mov [regs + _r10], r10
        mov [regs + _r11], r11
        mov [regs + _r12], r12
        mov [regs + _r13], r13
        mov [regs + _r14], r14
        mov [regs + _r15], r15

        mov QWORD [regs + _rip], [rsp + 8*15]

        mov [regs + 8*15], rax      
        mov rax, [rsp + 8*16]					; CS guardado por la CPU
        mov [regs + 8*16], rax      
        mov rax, [rsp + 8*17] 				  	; RFLAGS guardado por la CPU
        mov [regs + 8*17], rax		  
        mov rax, [rsp + 8*18]				   	; RSP guardado por la CPU
        mov [regs + 8*18], rax    
        mov rax, [rsp + 8*19]					; SS guardado por la CPU
        mov [regs + 8*19], rax

        mov rdi, %1                             ; Parametros para exceptionDispatcher
        mov rsi, regs

        call exceptionDispatcher

        popState
        call getStackBase
        mov [rsp+24], rax ; El StackBase
        mov rax, userland
        mov [rsp], rax ; PISO la dirección de retorno

        sti
        iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn

;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5


;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid Opcode Exception
_exception6Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret



SECTION .bss
	aux resq 1
