#include <colors.h>
#include <naiveConsole.h>

#define ZERO_EXCEPTION_ID 0x00
#define INVALID_OPCODE_ID 0x06

static void zero_division();
static void invalid_opcode();

void exceptionDispatcher(int exception) {
        if (exception == ZERO_EXCEPTION_ID)
                zero_division();
        else if (exception == INVALID_OPCODE_ID)
                invalid_opcode();
}

static void zero_division() {
        ncPrint("\n========================================\n", RED);
        ncPrint("EXCEPTION 0: DIVISION BY ZERO\n", RED);
        ncPrint("A division by zero was attempted.\n", RED);
        ncPrint("The system has been halted.\n", RED);
        ncPrint("========================================\n", RED);

        // Halt the CPU
        __asm__ volatile("cli; hlt");
}

static void invalid_opcode() {
        ncPrint("\n========================================\n", RED);
        ncPrint("EXCEPTION 6: INVALID OPCODE\n", RED);
        ncPrint("An invalid instruction was executed.\n", RED);
        ncPrint("The system has been halted.\n", RED);
        ncPrint("========================================\n", RED);

        // Halt the CPU
        __asm__ volatile("cli; hlt");
}
