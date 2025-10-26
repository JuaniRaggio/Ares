#include <defs.h>
#include <arch/x86_64/idtLoader.h>
#include <arch/x86_64/interrupts.h>
#include <stdint.h>

#include <drivers/keyboard_driver.h>

//Interruptions id
#define ID_TIMER_TICK 0x20
#define ID_KEYBOARD 0x21
#define ID_SYSCALL 0x80

//Exceptions id
#define ID_DIVISION_BY_ZERO 0x00

#pragma pack(push) /* Push de la alineación actual */
#pragma pack(1)    /* Alinear las siguiente estructuras a 1 byte */

/* Descriptor de interrupcion */
typedef struct {
        uint16_t offset_l, selector;
        uint8_t cero, access;
        uint16_t offset_m;
        uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop) /* Reestablece la alinceación actual */

DESCR_INT *idt = (DESCR_INT *)0; // IDT de 255 entradas

static void setup_IDT_entry(int index, uint64_t offset);

void load_idt() {

        //Interrupciones de software
        setup_IDT_entry(ID_TIMER_TICK, (uint64_t)&_irq00Handler);
        setup_IDT_entry(ID_KEYBOARD, (uint64_t)&_irq01Handler);
        
        //Interrupciones de hardware
        setup_IDT_entry(ID_SYSCALL, (uint64_t)&_syscallHandler);

        //Excepciones
        setup_IDT_entry(ID_DIVISION_BY_ZERO, (uint64_t)&_exception0Handler);

        // Solo interrupcion timer tick habilitadas
        picMasterMask(0xFC);
        picSlaveMask(0xFF);

        _sti();
}

static void setup_IDT_entry(int index, uint64_t offset) {
        idt[index].selector   = 0x08;
        idt[index].offset_l   = offset & 0xFFFF;
        idt[index].offset_m   = (offset >> 16) & 0xFFFF;
        idt[index].offset_h   = (offset >> 32) & 0xFFFFFFFF;
        idt[index].access     = ACS_INT;
        idt[index].cero       = 0;
        idt[index].other_cero = (uint64_t)0;
}
