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
        // Los registros ya fueron guardados en regs_buffer por el assembly
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
        ncPrint("  RBX: 0x", VGA_WHITE);
        ncPrintHex(regs->rbx);
        ncPrint("  RCX: 0x", VGA_WHITE);
        ncPrintHex(regs->rcx);
        ncNewline();

        ncPrint("RDX: 0x", VGA_WHITE);
        ncPrintHex(regs->rdx);
        ncPrint("  RSI: 0x", VGA_WHITE);
        ncPrintHex(regs->rsi);
        ncPrint("  RDI: 0x", VGA_WHITE);
        ncPrintHex(regs->rdi);
        ncNewline();

        ncPrint("RBP: 0x", VGA_WHITE);
        ncPrintHex(regs->rbp);
        ncPrint("  RSP: 0x", VGA_WHITE);
        ncPrintHex(regs->rsp);
        ncPrint("  RIP: 0x", VGA_WHITE);
        ncPrintHex(regs->rip);
        ncNewline();

        ncPrint("R8:  0x", VGA_WHITE);
        ncPrintHex(regs->r8);
        ncPrint("  R9:  0x", VGA_WHITE);
        ncPrintHex(regs->r9);
        ncPrint("  R10: 0x", VGA_WHITE);
        ncPrintHex(regs->r10);
        ncNewline();

        ncPrint("R11: 0x", VGA_WHITE);
        ncPrintHex(regs->r11);
        ncPrint("  R12: 0x", VGA_WHITE);
        ncPrintHex(regs->r12);
        ncPrint("  R13: 0x", VGA_WHITE);
        ncPrintHex(regs->r13);
        ncNewline();

        ncPrint("R14: 0x", VGA_WHITE);
        ncPrintHex(regs->r14);
        ncPrint("  R15: 0x", VGA_WHITE);
        ncPrintHex(regs->r15);
        ncNewline();

        ncPrint("CS:     0x", VGA_WHITE);
        ncPrintHex(regs->cs);
        ncPrint("  SS:     0x", VGA_WHITE);
        ncPrintHex(regs->ss);
        ncNewline();

        ncPrint("RFLAGS: 0x", VGA_WHITE);
        ncPrintHex(regs->rflags);
        ncNewline();

        ncPrint("\nPress any key to continue...", VGA_CYAN);

        char buffer;
        picMasterMask(0xFD);
        _sti();
        while (sys_read(STDIN, &buffer, 1) == 0)
                ;
        _cli();
        picMasterMask(0xFF);
}
