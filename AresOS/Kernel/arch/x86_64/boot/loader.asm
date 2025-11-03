global loader
extern main
extern initializeKernelBinary
extern haltcpu

loader:
	call initializeKernelBinary	; Set up the kernel binary, and get thet stack address
	mov rsp, rax				; Set up the stack with the returned address
	call main
	call haltcpu
hang:
	cli
	hlt	; halt machine should kernel return
	jmp hang
