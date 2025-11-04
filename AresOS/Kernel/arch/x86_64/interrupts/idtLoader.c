#include <arch/x86_64/idtLoader.h>
#include <arch/x86_64/interrupts.h>
#include <defs.h>

#include <drivers/keyboard_driver.h>

// Hardware interrupt IDs (IRQs remapped to 0x20-0x2F)
#define ID_TIMER_TICK 0x20
#define ID_KEYBOARD 0x21

// Software interrupt ID (legacy, not used - we use SYSCALL instruction)
#define ID_SYSCALL 0x80

// Exception IDs
#define ID_DIVISION_BY_ZERO 0x00
#define ID_INVALID_OPCODE 0x06

#pragma pack(push) /* Push current alignment */
#pragma pack(1)    /* Align following structures to 1 byte */

/* Interrupt descriptor */
typedef struct {
        uint16_t offset_l, selector;
        uint8_t cero, access;
        uint16_t offset_m;
        uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop) /* Restore alignment */

DESCR_INT *idt = (DESCR_INT *)0;

static void setup_IDT_entry(int index, uint64_t offset);

void load_idt() {
        setup_IDT_entry(ID_TIMER_TICK, (uint64_t)&_irq00Handler);
        setup_IDT_entry(ID_KEYBOARD, (uint64_t)&_irq01Handler);
        setup_IDT_entry(ID_DIVISION_BY_ZERO, (uint64_t)&_exception0Handler);
        setup_IDT_entry(ID_INVALID_OPCODE, (uint64_t)&_exception6Handler);
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
