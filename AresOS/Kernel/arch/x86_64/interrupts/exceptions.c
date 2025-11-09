#include "interrupts.h"
#include "syscalls.h"
#include <colors.h>
#include <naiveConsole.h>
#include <regs.h>

#define ZERO_EXCEPTION_ID 0x00
#define INVALID_OPCODE_ID 0x06

extern regs_snapshot_t saved_regs;

static void zero_division(regs_snapshot_t *regs);
static void invalid_opcode(regs_snapshot_t *regs);
static void print_registers(regs_snapshot_t *regs);

void exceptionDispatcher(int exception, regs_snapshot_t *regs) {
        saved_regs = *regs;

        if (exception == ZERO_EXCEPTION_ID)
                zero_division(regs);
        else if (exception == INVALID_OPCODE_ID)
                invalid_opcode(regs);

        print_registers(regs);
}

static void zero_division(regs_snapshot_t *regs) {
        ncPrint("\n========================================\n", VGA_RED);
        ncPrint("EXCEPTION 0: DIVISION BY ZERO\n", VGA_RED);
        ncPrint("A division by zero was attempted.\n", VGA_RED);
        ncPrint("========================================\n", VGA_RED);
}

static void invalid_opcode(regs_snapshot_t *regs) {
        ncPrint("\n========================================\n", VGA_RED);
        ncPrint("EXCEPTION 6: INVALID OPCODE\n", VGA_RED);
        ncPrint("An invalid instruction was executed.\n", VGA_RED);
        ncPrint("========================================\n", VGA_RED);
}

static void print_registers(regs_snapshot_t *regs) {
        ncPrint("\nREGISTER STATE:\n", VGA_CYAN);

        ncPrint("RAX: 0x", VGA_WHITE);
        ncPrintHex(regs->rax);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  RBX: 0x", VGA_WHITE);
        ncPrintHex(regs->rbx);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  RCX: 0x", VGA_WHITE);
        ncPrintHex(regs->rcx);
        ncPrint("\n", VGA_WHITE);

        ncPrint("RDX: 0x", VGA_WHITE);
        ncPrintHex(regs->rdx);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  RSI: 0x", VGA_WHITE);
        ncPrintHex(regs->rsi);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  RDI: 0x", VGA_WHITE);
        ncPrintHex(regs->rdi);
        ncPrint("\n", VGA_WHITE);

        ncPrint("RBP: 0x", VGA_WHITE);
        ncPrintHex(regs->rbp);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  RSP: 0x", VGA_WHITE);
        ncPrintHex(regs->rsp);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  RIP: 0x", VGA_WHITE);
        ncPrintHex(regs->rip);
        ncPrint("\n", VGA_WHITE);

        ncPrint("R8:  0x", VGA_WHITE);
        ncPrintHex(regs->r8);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  R9:  0x", VGA_WHITE);
        ncPrintHex(regs->r9);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  R10: 0x", VGA_WHITE);
        ncPrintHex(regs->r10);
        ncPrint("\n", VGA_WHITE);

        ncPrint("R11: 0x", VGA_WHITE);
        ncPrintHex(regs->r11);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  R12: 0x", VGA_WHITE);
        ncPrintHex(regs->r12);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  R13: 0x", VGA_WHITE);
        ncPrintHex(regs->r13);
        ncPrint("\n", VGA_WHITE);

        ncPrint("R14: 0x", VGA_WHITE);
        ncPrintHex(regs->r14);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  R15: 0x", VGA_WHITE);
        ncPrintHex(regs->r15);
        ncPrint("\n", VGA_WHITE);

        ncPrint("CS:     0x", VGA_WHITE);
        ncPrintHex(regs->cs);
        ncPrint("\n", VGA_WHITE);
        ncPrint("  SS:     0x", VGA_WHITE);
        ncPrintHex(regs->ss);
        ncPrint("\n", VGA_WHITE);

        ncPrint("RFLAGS: 0x", VGA_WHITE);
        ncPrintHex(regs->rflags);
        ncPrint("\n", VGA_WHITE);

        ncPrint("\nPress any key to continue...", VGA_CYAN);

        char buffer;
        picMasterMask(PIC_MASK_KBD_ONLY);
        _sti();
        while (sys_read(STDIN, &buffer, 1) == 0)
                ;
        picMasterMask(PIC_MASK_TIMER_KBD);
}
