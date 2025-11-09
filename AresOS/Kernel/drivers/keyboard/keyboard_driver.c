#include "colors.h"
#include "naiveConsole.h"
#include <drivers/keyboard_driver.h>
#include <regs.h>

typedef struct {
        uint8_t buffer[TABLE_SIZE];
        uint8_t write_pos; // head
        uint8_t read_pos;  // tail
        uint8_t modifiers; // off, Shift, Ctrl, Alt
} keyboard_state_t;

static keyboard_state_t keyboard = {0};

/* External variable defined in syscalls.c */
extern regs_snapshot_t saved_regs;

uint8_t keyboard_handler(uint64_t *stack_ptr) {
        uint8_t scan_code = get_input();

        // BREAK_CODE -> Key released
        if (scan_code & BREAK_CODE) {
                uint8_t make_code = scan_code & 0x7F; // Remove bit 7

                if (make_code == LSHIFT_CODE || make_code == RSHIFT_CODE) {
                        keyboard.modifiers = (uint8_t)off; // Shift released
                }
                if (make_code == LCTRL_CODE) {
                        keyboard.modifiers = (uint8_t)off; // Ctrl released
                }
                goto end;
        }
        // MAKE_CODE -> Key pressed
        if (scan_code == LSHIFT_CODE || scan_code == RSHIFT_CODE) {
                keyboard.modifiers = (uint8_t)shift; // Shift pressed
                goto end;
        }

        if (scan_code == LCTRL_CODE) {
                keyboard.modifiers = (uint8_t)ctl; // Ctrl pressed
                goto end;
        }

        // Hotkey to capture registers (Ctrl+R)
        if (scan_code == R_CODE && keyboard.modifiers == ctl) {
                capture_registers(stack_ptr);
                goto end;
        }

        if (scan_code == MINUS_CODE && keyboard.modifiers == ctl) {
                return ZOOM_OUT_CHAR;
        }

        if (scan_code == EQUALS_CODE && keyboard.modifiers == ctl) {
                return ZOOM_IN_CHAR;
        }

        return ascii_table[keyboard.modifiers][scan_code];
end:
        return 0;
}

void update_buffer(uint8_t c) {
        keyboard.buffer[keyboard.write_pos++] = c;
}

uint8_t buffer_has_next() {
        return keyboard.write_pos != keyboard.read_pos;
}

uint8_t buffer_next() {
        if (!buffer_has_next()) {
                return 0; // No char in buffer
        }

        uint8_t aux = keyboard.buffer[keyboard.read_pos];

        keyboard.read_pos = (keyboard.read_pos + 1) & 0xff;

        return aux;
}

void capture_registers(uint64_t *stack_ptr) {

        saved_regs.r15 = stack_ptr[0];
        saved_regs.r14 = stack_ptr[1];
        saved_regs.r13 = stack_ptr[2];
        saved_regs.r12 = stack_ptr[3];
        saved_regs.r11 = stack_ptr[4];
        saved_regs.r10 = stack_ptr[5];
        saved_regs.r9  = stack_ptr[6];
        saved_regs.r8  = stack_ptr[7];
        saved_regs.rsi = stack_ptr[8];
        saved_regs.rdi = stack_ptr[9];
        saved_regs.rbp = stack_ptr[10];
        saved_regs.rdx = stack_ptr[11];
        saved_regs.rcx = stack_ptr[12];
        saved_regs.rbx = stack_ptr[13];
        saved_regs.rax = stack_ptr[14];

        saved_regs.rip    = stack_ptr[15]; // RIP
        saved_regs.cs     = stack_ptr[16]; // CS
        saved_regs.rflags = stack_ptr[17]; // RFLAGS
        saved_regs.rsp    = stack_ptr[18]; // RSP
        saved_regs.ss     = stack_ptr[19]; // SS

        ncPrint("\n[Registers captured with Ctrl+R]\n", VGA_GREEN);
}
