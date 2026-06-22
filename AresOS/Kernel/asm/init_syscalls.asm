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
    ; Bits 63:48 = base para SYSRET: CS = base+16 (0x20|3 = user code)
    ;              y SS = base+8 (0x18|3 = user data)
    ; Bits 47:32 = base para SYSCALL: CS = 0x08 (kernel code), SS = 0x10
    mov ecx, 0xC0000081          ; IA32_STAR
    xor eax, eax                 ; Bits 31:0 = 0 (no usados)
    mov edx, 0x00100008          ; EDX = [SYSRET base : SYSCALL base]
    wrmsr

    ; 3. LSTAR: Dirección del handler (64-bit)
    mov ecx, 0xC0000082          ; IA32_LSTAR
    lea rax, [rel syscall_entry] ; Dirección del handler
    mov rdx, rax
    shr rdx, 32                  ; RDX = bits 63:32
    wrmsr

    mov ecx, 0xC0000084
    xor eax, eax
    xor edx, edx
    wrmsr

    ret
