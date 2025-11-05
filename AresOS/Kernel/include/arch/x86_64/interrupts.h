#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include <idtLoader.h>
#include <stdint.h>

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
