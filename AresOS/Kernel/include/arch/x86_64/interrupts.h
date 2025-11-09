#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include <idtLoader.h>
#include <stdint.h>

/**
 * PIC (Programmable Interrupt Controller) Mask Definitions
 *
 * In PIC masks, a bit set to 1 means the IRQ is DISABLED
 * A bit set to 0 means the IRQ is ENABLED
 *
 * Individual IRQ bits
 */
#define PIC_IRQ0_TIMER (1 << 0)     // 0x01 - System Timer
#define PIC_IRQ1_KEYBOARD (1 << 1)  // 0x02 - Keyboard
#define PIC_IRQ2_CASCADE (1 << 2)   // 0x04 - Cascade (never masked)
#define PIC_IRQ3_SERIAL_24 (1 << 3) // 0x08 - Serial Port 2 and 4
#define PIC_IRQ4_SERIAL_13 (1 << 4) // 0x10 - Serial Port 1 and 3
#define PIC_IRQ5_USB (1 << 5)       // 0x20 - USB
#define PIC_IRQ6 (1 << 6)           // 0x40 - IRQ6
#define PIC_IRQ7 (1 << 7)           // 0x80 - IRQ7

/**
 * Common PIC mask configurations
 */
#define PIC_MASK_ALL 0xFF       // All IRQs disabled
#define PIC_MASK_NONE 0x00      // All IRQs enabled
#define PIC_MASK_TIMER_KBD 0xFC // Only Timer (IRQ0) and Keyboard (IRQ1) enabled
#define PIC_MASK_KBD_ONLY 0xFD  // Only Keyboard (IRQ1) enabled
#define PIC_MASK_TIMER_ONLY 0xFE // Only Timer (IRQ0) enabled

/**
 * Exception codes
 */
#define ZERO_EXCEPTION_ID 0x00
#define INVALID_OPCODE_ID 0x06

/**
 * System call interrupt handler
 */
void _syscallHandler(void);

/**
 * IRQ 0 handler - 8254 Timer (Timer Tick)
 */
void _irq00Handler(void);

/**
 * IRQ 1 handler - Keyboard
 */
void _irq01Handler(void);

/**
 * IRQ 2 handler - Cascade PIC (never called)
 */
void _irq02Handler(void);

/**
 * IRQ 3 handler - Serial Port 2 and 4
 */
void _irq03Handler(void);

/**
 * IRQ 4 handler - Serial Port 1 and 3
 */
void _irq04Handler(void);

/**
 * IRQ 5 handler - USB
 */
void _irq05Handler(void);

/**
 * Exception 0 handler - Zero Division Exception
 */
void _exception0Handler(void);

/**
 * Exception 6 handler - Invalid Opcode Exception
 */
void _exception6Handler(void);

/**
 * Clear interrupt flag (disable interrupts)
 */
void _cli(void);

/**
 * Set interrupt flag (enable interrupts)
 */
void _sti(void);

/**
 * Halt the CPU until next interrupt
 */
void _hlt(void);

/**
 * Sets the interrupt mask for the master PIC
 * @param mask Interrupt mask to apply
 */
void picMasterMask(uint8_t mask);

/**
 * Sets the interrupt mask for the slave PIC
 * @param mask Interrupt mask to apply
 */
void picSlaveMask(uint8_t mask);

/**
 * Halts the CPU and stops execution (disables interrupts first)
 */
void haltcpu(void);

#endif /* INTERRUPS_H_ */
