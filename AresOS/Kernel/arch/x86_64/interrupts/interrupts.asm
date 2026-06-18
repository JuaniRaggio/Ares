;interrupts.asm
GLOBAL _cli
GLOBAL _sti
GLOBAL irq_save
GLOBAL irq_restore
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
GLOBAL _irq08Handler


GLOBAL _exception0Handler
GLOBAL _exception6Handler

GLOBAL _irq81Handler
GLOBAL _yield_now

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN getStackBase
EXTERN schedule
EXTERN do_yield_switch

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

	mov rdi, %1 ; parameter passing
	mov rsi, rsp ; pass stack pointer for register capture
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

%macro irqHandlerSlave 1
	pushState

	mov rdi, %1 ; parameter passing
	mov rsi, rsp ; pass stack pointer for register capture
	call irqDispatcher

	; signal pic EOI to both slave and master
	mov al, 20h
	out 0A0h, al  ; Slave PIC
	out 20h, al   ; Master PIC

	popState
	iretq
%endmacro



%macro exceptionHandler 1
        cli
        pushState

        ; Copy registers from stack to regs_buffer safely
        mov rsi, rsp                ; Source = stack
        lea rdi, [regs_buffer]      ; Destination = buffer
        mov rcx, 20                 ; 20 registers (qwords)
        rep movsq                   ; Copy efficiently

        ; Call exceptionDispatcher
        mov rdi, %1                 ; Exception number
        lea rsi, [regs_buffer]      ; Pointer to safe copy

        call exceptionDispatcher

        popState
        call getStackBase
        mov [rsp+24], rax            ; The StackBase
        mov rax, userland
        mov [rsp], rax               ; Overwrite return address

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

; uint64_t irq_save(void)
; Returns the current RFLAGS and disables interrupts. Pairs with
; irq_restore to nest interrupt-off regions safely: an inner region
; restores to "disabled" if the outer one already had them off, instead
; of re-enabling early like a bare cli/sti would.
irq_save:
	pushfq
	pop rax
	cli
	ret

; void irq_restore(uint64_t flags)
; Restores RFLAGS saved by irq_save (re-enables interrupts only if they
; were enabled at save time).
irq_restore:
	push rdi
	popfq
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

;8254 Timer (Timer Tick) - custom handler with context switch
_irq00Handler:
	pushState

	mov rdi, rsp
	call schedule
	mov rsp, rax

	mov al, 20h
	out 20h, al

	popState
	iretq

;Cooperative yield: software-triggered context switch (vector 0x81).
;Same switch path as the timer but without timekeeping or PIC EOI, so a
;process can give up the CPU immediately from inside a syscall. iretq adapts
;to the saved frame (ring0 if the process parked here mid-syscall, ring3 if it
;came from userland), so it composes with timer-driven switches.
_irq81Handler:
	pushState
	mov rdi, rsp
	call do_yield_switch
	mov rsp, rax
	popState
	iretq

;void _yield_now(void): trigger an immediate reschedule from kernel code.
_yield_now:
	int 81h
	ret

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

;RTC (Real-Time Clock) - IRQ 8
_irq08Handler:
	irqHandlerSlave 8

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
regs_buffer: resb 160  ; Buffer para almacenar registros (20 registros x 8 bytes)

SECTION .rodata
	userland equ 0x400000
