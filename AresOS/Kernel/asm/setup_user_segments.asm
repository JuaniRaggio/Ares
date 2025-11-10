; setup_user_segments.asm
; Carga una nueva GDT con segmentos de usuario

BITS 64
global setup_user_segments
extern tss64
extern tss_size

section .data
align 16
gdt64_new:
    ; Entrada 0: Null descriptor
    dq 0x0000000000000000

    ; Entrada 1 (0x08): Kernel Code Segment
    ; Base=0, Limit=0, Type=Code, DPL=0, L=1 (64-bit)
    dq 0x00209A0000000000

    ; Entrada 2 (0x10): Kernel Data Segment
    ; Base=0, Limit=0, Type=Data, DPL=0
    dq 0x0000920000000000

    ; Entrada 3 (0x18): User Code Segment
    ; Base=0, Limit=0, Type=Code, DPL=3, L=1 (64-bit)
    dq 0x0020FA0000000000

    ; Entrada 4 (0x20): User Data Segment
    ; Base=0, Limit=0, Type=Data, DPL=3
    dq 0x0000F20000000000

; Entrada 5 (0x28): TSS Descriptor (16 bytes en 64-bit)
; Este descriptor requiere 2 entradas de 8 bytes
tss_descriptor:
    dw tss_size - 1          ; Limit (bits 0-15): tamaño del TSS - 1
    dw 0x0000                ; Base (bits 0-15) - se llenara en runtime
    db 0x00                  ; Base (bits 16-23)
    db 0x89                  ; Type = Available TSS (0x9), Present (0x80)
    db 0x00                  ; Limit (bits 16-19) + flags
    db 0x00                  ; Base (bits 24-31)
    dd 0x00000000            ; Base (bits 32-63)
    dd 0x00000000            ; Reserved

gdt64_new_end:

gdt64_ptr:
    dw gdt64_new_end - gdt64_new - 1  ; Limit
    dq gdt64_new                       ; Base

section .text

; void setup_user_segments(void);
setup_user_segments:
; Escribir la direccion base del TSS en el descriptor
    lea rax, [rel tss64]
    mov [rel tss_descriptor + 2], ax       ; Base bits 0-15
    shr rax, 16
    mov [rel tss_descriptor + 4], al       ; Base bits 16-23
    shr rax, 8
    mov [rel tss_descriptor + 7], al       ; Base bits 24-31
    shr rax, 8
    mov [rel tss_descriptor + 8], eax      ; Base bits 32-63
    ; Cargar la nueva GDT
    lgdt [rel gdt64_ptr]

    ; Recargar segmentos de kernel
    mov ax, 0x10      ; Kernel Data Segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; No necesitamos recargar CS porque seguimos en kernel mode
    ; CS se recargara automaticamente con el far return o iretq

    ret
