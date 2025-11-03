; ============================================
; init_syscalls.asm
; Configura los registros MSR para SYSCALL/SYSRET
; ============================================

global init_syscalls
extern syscall_entry

section .text

init_syscalls:
    ; 1. Habilitar SYSCALL en EFER (bit 0 = SCE)
    mov ecx, 0xC0000080          ; IA32_EFER
    rdmsr
    or eax, 1                    ; Bit 0 = SCE (System Call Extensions)
    wrmsr

    ; 2. STAR: Configurar selectores de segmento
    ; Bits 63:48 = User CS (0x1B = selector 3, RPL=3, para SYSRET)
    ; Bits 47:32 = Kernel CS (0x08 = selector 1, RPL=0, para SYSCALL)
    mov ecx, 0xC0000081          ; IA32_STAR
    xor eax, eax                 ; Bits 31:0 = 0 (no usados)
    mov edx, 0x00180008          ; EDX = [User CS : Kernel CS]
    ; 0x0018 en bits altos = user CS base (SYSRET usa +16 para SS)
    ; 0x0008 en bits bajos = kernel CS
    wrmsr

    ; 3. LSTAR: Dirección del handler (64-bit)
    mov ecx, 0xC0000082          ; IA32_LSTAR
    lea rax, [rel syscall_entry] ; Dirección del handler
    mov rdx, rax
    shr rdx, 32                  ; RDX = bits 63:32
    wrmsr

    ; 4. FMASK: Máscara de RFLAGS (flags a limpiar al entrar)
    mov ecx, 0xC0000084          ; IA32_FMASK
    mov eax, 0x00000200          ; Bit 9 = IF (Interrupt Flag)
    ; mov eax, 0x00000700         ; IF | DF | TF
    xor edx, edx
    wrmsr

    ret
