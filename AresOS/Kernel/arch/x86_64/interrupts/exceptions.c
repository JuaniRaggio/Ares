#include "interrupts.h"
#include "syscalls.h"
#include <colors.h>
#include <naiveConsole.h>
#include <process.h>
#include <regs.h>
#include <stdint.h>

extern regs_snapshot_t saved_regs;

static void zero_division(regs_snapshot_t *regs);
static void invalid_opcode(regs_snapshot_t *regs);
static void report_fault(pid_t pid);
static void print_registers(regs_snapshot_t *regs);
static void wait_for_keypress(void);

void exceptionDispatcher(int exception, regs_snapshot_t *regs) {
        saved_regs = *regs;

        pid_t pid = process_getpid();

        if (exception == ZERO_EXCEPTION_ID)
                zero_division(regs);
        else if (exception == INVALID_OPCODE_ID)
                invalid_opcode(regs);

        /* Registers first (debugging detail), then the human-readable verdict
         * last so it stays on screen even if the long dump scrolls the top off
         * (the console clears on overflow). */
        print_registers(regs);
        report_fault(pid);

        ncPrint("\nPress any key to continue...", VGA_CYAN);
        wait_for_keypress();

        /* Kill the faulting process instead of resuming it. process_exit never
         * returns, so the handler's tail (iretq to userland 0x400000) only runs
         * for the shell, which is still restarted in place. */
        if (pid != SHELL_PID)
                process_exit(KILLED_EXIT_CODE);
}

/* Tells the user which process faulted and what the kernel did about it: feed-
 * back for the operator and a debugging anchor for us. */
static void report_fault(pid_t pid) {
        pcb_t *pcb = process_get_current();

        ncPrint("\nFaulting process: ", VGA_CYAN);
        ncPrint(pcb != NULL ? pcb->name : "unknown", VGA_WHITE);
        ncPrint(" (pid ", VGA_CYAN);
        ncPrintDec((uint64_t)pid);
        ncPrint(")\n", VGA_CYAN);

        if (pid == SHELL_PID)
                ncPrint("Action: shell recovered and restarted.\n", VGA_GREEN);
        else
                ncPrint("Action: process terminated by the kernel; "
                        "the rest of the system keeps running.\n",
                        VGA_GREEN);
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
}

static void wait_for_keypress(void) {
        char buffer;
        uint64_t byte;
        picMasterMask(PIC_MASK_KBD_ONLY);
        _sti();
        do {
                byte = 1;
        } while (sys_read(STDIN, &buffer, &byte) != 0 || byte == 0);
        picMasterMask(PIC_MASK_TIMER_KBD);
}
