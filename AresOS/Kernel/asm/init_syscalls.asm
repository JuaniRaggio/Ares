; ============================================
; init_syscalls.asm
; Configura los registros MSR para SYSCALL/SYSRET
; ============================================

global init_syscalls
extern syscall_entry

section .text
init_syscalls:
    ; 1. Asegurar que SYSCALL/SYSRET esté habilitado
    mov ecx, 0xC0000080          ; EFER
    rdmsr
    or eax, 1                    ; Bit 0 = SCE
    wrmsr

    ; 2. STAR: define selectores de segmento user/kernel
    mov ecx, 0xC0000081          ; STAR
    mov edx, 0x001B0008          ; [63:48]=0x1B (userCS), [47:32]=0x08 (kernelCS)
    xor eax, eax
    wrmsr

    ; 3. LSTAR: dirección del handler de syscall
    mov ecx, 0xC0000082          ; LSTAR
    mov rax, syscall_entry
    mov rdx, rax                 ; copiar rax en rdx
    shr rdx, 32                  ; sacar la parte alta
    nop
    wrmsr

    ; 4. FMASK: flags a limpiar al entrar al kernel
    mov ecx, 0xC0000084          ; FMASK
    mov eax, (1 << 9)            ; limpia IF (bit 9)
    xor edx, edx
    wrmsr

    ret

