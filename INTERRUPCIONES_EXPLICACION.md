# Explicación: Interrupciones en Pure64 vs Kernel

## Tu pregunta es MUY válida

Tienes razón en notar que Pure64 ya implementa interrupciones. Aquí te explico **qué hace Pure64, qué debes mantener, y qué debes reimplementar**.

---

## ¿Qué hace Pure64 con las interrupciones?

### 1. Configura una IDT TEMPORAL y BÁSICA

En `pure64.asm` (líneas 332-406), Pure64:

```asm
; Build a temporary IDT
xor rdi, rdi  ; IDT en dirección 0x0000000000000000

; Crea 32 exception gates (0-31)
mov rcx, 32
make_exception_gates:
    mov rax, exception_gate  ; Handler genérico
    ; ... configura la entrada de la IDT ...

; Crea 224 interrupt gates (32-255)
mov rcx, 256-32
make_interrupt_gates:
    mov rax, interrupt_gate  ; Handler genérico
    ; ... configura la entrada de la IDT ...

; Configura handlers específicos
mov rdi, 0x21  ; IRQ1 - Keyboard
mov rax, keyboard
call create_gate

mov rdi, 0x28  ; IRQ8 - RTC
mov rax, rtc
call create_gate

lidt [IDTR64]  ; Carga la IDT
```

### 2. Los handlers son MINIMALISTAS

Mira los handlers en `interrupt.asm`:

**Exception handler (línea 11-19):**
```asm
exception_gate:
    mov rsi, int_string
    call os_print_string
    mov rsi, exc_string
    call os_print_string
exception_gate_halt:
    cli
    hlt
    jmp exception_gate_halt  ; SE CUELGA EL SISTEMA
```

**Interrupt handler genérico (línea 25):**
```asm
interrupt_gate:
    iretq  ; NO HACE NADA, solo retorna
```

**Keyboard handler (línea 34-56):**
```asm
keyboard:
    push rdi
    push rax
    in al, 0x60           ; Lee scancode
    test al, 0x80
    jnz keyboard_done
    mov [0x000B8088], al  ; LO ESCRIBE DIRECTO EN PANTALLA (!!!)
    mov al, 0x20
    out 0x20, al          ; Envía EOI
    pop rax
    pop rdi
    iretq
```

---

## ¿Por qué NO puedes usar estos handlers?

### Problema 1: Los handlers son para DEBUGGING del bootloader
- Escriben directamente en memoria VGA (`0x000B8088`)
- No tienen estructura de buffer
- No son adecuados para un sistema operativo real

### Problema 2: Los exception handlers CUELGAN el sistema
- Si hay una división por cero, el sistema se detiene para siempre
- No hay recuperación
- No hay información útil para debugging

### Problema 3: Son TEMPORALES
- Pure64 los crea para que el bootloader no explote si hay una interrupción
- Están pensados para durar **solo hasta que tu kernel tome control**

---

## ¿Qué DEBES hacer en tu kernel?

### Opción 1: REEMPLAZAR completamente la IDT (RECOMENDADO)

**Ventajas:**
- Control total
- Handlers apropiados para un OS
- Puedes usar estructuras de datos en C

**Pasos:**
1. En `kernel_main()`, crear tu propia IDT en otra ubicación (ej: 0x5000)
2. Implementar tus propios handlers que:
   - Guarden contexto completo (todos los registros)
   - Llamen a funciones en C para procesamiento
   - Restauren contexto y retornen
3. Llamar a `lidt` con tu nueva IDT
4. La IDT de Pure64 queda obsoleta

**Ejemplo:**
```c
// En tu kernel
void kernel_main(void) {
    // ...
    idt_init();  // Crea TU IDT desde cero
    // Desde este punto, usas TUS handlers
}
```

### Opción 2: EXTENDER la IDT de Pure64 (MÁS COMPLEJO)

**Solo si quieres reusar la IDT en 0x0:**
1. Mantener la IDT en 0x0
2. Sobrescribir los handlers que necesites usando `create_gate`
3. Dejar intactos los que no uses

**NO recomendado porque:**
- Código más frágil
- Dependencia de Pure64
- Más difícil de debuggear

---

## ¿Qué PUEDES reusar de Pure64?

### 1. La función `create_gate` (syscalls.asm, línea 329)
```asm
; rax = dirección del handler
; rdi = número de gate (0-255)
create_gate:
    push rdi
    push rax
    shl rdi, 4
    stosw
    shr rax, 16
    add rdi, 4
    stosw
    shr rax, 16
    stosd
    pop rax
    pop rdi
    ret
```

**Es útil para agregar handlers a la IDT.** Puedes copiarla a tu kernel.

### 2. La inicialización del PIC (pic.asm)
```asm
init_pic:
    ; Habilita Keyboard y RTC
    in al, 0x21
    mov al, 11111001b
    out 0x21, al
    in al, 0xA1
    mov al, 11111110b
    out 0xA1, al
    ; ...
    sti
    ret
```

**Pero necesitas REMAPEAR el PIC** para que IRQ0 vaya a INT 32 en lugar de INT 8:

```asm
; Remapeo del PIC (lo que falta en Pure64)
mov al, 0x11
out 0x20, al    ; ICW1: iniciar PIC maestro
out 0xA0, al    ; ICW1: iniciar PIC esclavo

mov al, 0x20    ; IRQ0-7 van a INT 32-39
out 0x21, al    ; ICW2: offset del maestro
mov al, 0x28    ; IRQ8-15 van a INT 40-47
out 0xA1, al    ; ICW2: offset del esclavo

mov al, 0x04
out 0x21, al    ; ICW3: maestro tiene esclavo en IRQ2
mov al, 0x02
out 0xA1, al    ; ICW3: esclavo conectado a IRQ2

mov al, 0x01
out 0x21, al    ; ICW4: modo 8086
out 0xA1, al
```

**NOTA:** Pure64 NO remapea el PIC, por eso:
- IRQ1 (teclado) está en INT 0x21 (33)
- IRQ8 (RTC) está en INT 0x28 (40)

Esto funciona porque **por casualidad** no colisionan con excepciones (0-31). Pero es mejor remapear correctamente.

---

## Plan de acción recomendado

### Fase 1: Crear tu propia IDT

```c
// kernel/idt.c
#define IDT_ENTRIES 256

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed)) idt_entry_t;

idt_entry_t idt[IDT_ENTRIES];

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idtr_t;

idtr_t idtr;

void idt_set_gate(uint8_t num, uint64_t handler, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].selector = 0x08;  // Código del kernel
    idt[num].ist = 0;
    idt[num].type_attr = flags;
    idt[num].offset_mid = (handler >> 16) & 0xFFFF;
    idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[num].zero = 0;
}

void idt_init(void) {
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)&idt;

    // Limpiar IDT
    memset(&idt, 0, sizeof(idt));

    // Configurar excepciones (0-31)
    idt_set_gate(0, (uint64_t)isr0, 0x8E);
    idt_set_gate(1, (uint64_t)isr1, 0x8E);
    // ... hasta 31

    // Configurar IRQs (32-47 después de remapear PIC)
    idt_set_gate(32, (uint64_t)irq0, 0x8E);
    idt_set_gate(33, (uint64_t)irq1, 0x8E);  // Keyboard
    // ... hasta 47

    // Cargar IDT
    __asm__ volatile("lidt %0" : : "m"(idtr));
}
```

### Fase 2: Implementar handlers en Assembly

```asm
; kernel/isr.asm
global isr0
global isr1
; ...
global irq0
global irq1
; ...

extern exception_handler
extern irq_handler

; Macro para ISRs sin error code
%macro ISR_NOERR 1
isr%1:
    push 0          ; Error code falso
    push %1         ; Número de interrupción
    jmp isr_common
%endmacro

; Macro para ISRs con error code
%macro ISR_ERR 1
isr%1:
    push %1         ; Número de interrupción
    jmp isr_common
%endmacro

ISR_NOERR 0   ; Division by zero
ISR_NOERR 1   ; Debug
; ...
ISR_ERR 8     ; Double fault (tiene error code)
; ...

isr_common:
    ; Guardar todos los registros
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Llamar handler en C
    mov rdi, rsp  ; Puntero al stack (contexto)
    call exception_handler

    ; Restaurar registros
    pop r15
    pop r14
    ; ... resto

    add rsp, 16  ; Limpiar número de int y error code
    iretq
```

### Fase 3: Handlers en C

```c
// kernel/exception.c
void exception_handler(interrupt_frame_t* frame) {
    printf("Exception %d at RIP: 0x%llx\n",
           frame->int_no, frame->rip);

    // Mostrar info útil
    dump_registers(frame);

    // Intentar recuperar (volver a shell) en lugar de colgarse
    if (frame->int_no == 14) {  // Page fault
        handle_page_fault(frame);
    } else {
        printf("Sistema detenido. Presiona reset.\n");
        while(1) { asm("hlt"); }
    }
}
```

---

## Resumen final

| Componente | Pure64 | Tu Kernel |
|------------|--------|-----------|
| **IDT location** | 0x0 | 0x5000 (o donde quieras) |
| **Exception handlers** | Cuelgan sistema | Informativos, recuperables |
| **IRQ handlers** | Minimalistas | Completos con buffers |
| **Keyboard** | Escribe en pantalla | Buffer circular |
| **RTC** | Incrementa contador | Mantiene tiempo, delays |
| **PIC** | No remapeado | Remapeado a INT 32+ |

**Conclusión:** Pure64 te da interrupciones **solo para sobrevivir el booteo**. Tu kernel debe reemplazarlas completamente con handlers profesionales.

---

## Actualización al roadmap

Deberías modificar las tareas del roadmap:

**En lugar de:**
- "Configuración de IDT" $->$ CRITICA

**Debería ser:**
- "Crear nueva IDT del kernel (reemplazar la de Pure64)" $->$ CRITICA
- "Remapear el PIC correctamente" $->$ CRITICA
- "Implementar exception handlers informativos" $->$ CRITICA
- "Implementar IRQ handlers con buffers" $->$ ALTA

**Dependencias actualizadas:**
- La IDT de Pure64 es temporal
- Funciona mientras el kernel arranca
- Tu primera tarea en `kernel_main` debe ser reemplazarla

