#include <drivers/keyboard_driver.h>
#include <regs.h>

typedef enum {
        off,
        shift,
        ctl,
        alt,
} modifiers;

// This table either must be initialized matching the modifiers enum
// or should be assigned using modifiers as indexes
static char ascii_table[][TABLE_SIZE] = {
    // off - normal version
    {
        /* 0x00 */ NON_PRINTABLE,
        /* 0x01 */ NON_PRINTABLE, // Escape pressed (no ASCII)
        /* 0x02 */ '1',           // 1 pressed
        /* 0x03 */ '2',           // 2 pressed
        /* 0x04 */ '3',           // 3 pressed
        /* 0x05 */ '4',           // 4 pressed
        /* 0x06 */ '5',           // 5 pressed
        /* 0x07 */ '6',           // 6 pressed
        /* 0x08 */ '7',           // 7 pressed
        /* 0x09 */ '8',           // 8 pressed
        /* 0x0A */ '9',           // 9 pressed
        /* 0x0B */ '0',           // 0 (zero) pressed
        /* 0x0C */ '-',           // – pressed
        /* 0x0D */ '=',           // = pressed
        /* 0x0E */ '\b',          // Backspace pressed
        /* 0x0F */ '\t',          // Tab pressed
        /* 0x10 */ 'q',           // Q pressed
        /* 0x11 */ 'w',           // W pressed
        /* 0x12 */ 'e',           // E pressed
        /* 0x13 */ 'r',           // R pressed
        /* 0x14 */ 't',           // T pressed
        /* 0x15 */ 'y',           // Y pressed
        /* 0x16 */ 'u',           // U pressed
        /* 0x17 */ 'i',           // I pressed
        /* 0x18 */ 'o',           // O pressed
        /* 0x19 */ 'p',           // P pressed
        /* 0x1A */ '[',           // [ pressed
        /* 0x1B */ ']',           // ] pressed
        /* 0x1C */ '\n',          // Enter pressed
        /* 0x1D */ NON_PRINTABLE, // Left Control pressed (no ASCII)
        /* 0x1E */ 'a',           // A pressed
        /* 0x1F */ 's',           // S pressed
        /* 0x20 */ 'd',           // D pressed
        /* 0x21 */ 'f',           // F pressed
        /* 0x22 */ 'g',           // G pressed
        /* 0x23 */ 'h',           // H pressed
        /* 0x24 */ 'j',           // J pressed
        /* 0x25 */ 'k',           // K pressed
        /* 0x26 */ 'l',           // L pressed
        /* 0x27 */ ';',           // ; pressed
        /* 0x28 */ '\'',          // ' (single quote) pressed
        /* 0x29 */ '`',           // ` (back tick) pressed
        /* 0x2A */ NON_PRINTABLE, // Left Shift pressed
        /* 0x2B */ '\\',          // \ pressed
        /* 0x2C */ 'z',           // Z pressed
        /* 0x2D */ 'x',           // X pressed
        /* 0x2E */ 'c',           // C pressed
        /* 0x2F */ 'v',           // V pressed
        /* 0x30 */ 'b',           // B pressed
        /* 0x31 */ 'n',           // N pressed
        /* 0x32 */ 'm',           // M pressed
        /* 0x33 */ ',',           // , pressed
        /* 0x34 */ '.',           // . pressed
        /* 0x35 */ '/',           // / pressed
        /* 0x36 */ NON_PRINTABLE, // Right Shift pressed
        /* 0x37 */ '*',           // (keypad) * pressed
        /* 0x38 */ NON_PRINTABLE, // Left Alt pressed
        /* 0x39 */ ' ',           // space pressed
        /* 0x3A */ NON_PRINTABLE, // CapsLock pressed
        /* 0x3B */ NON_PRINTABLE, // F1 pressed
    },
    // Shift‐pressed version: ASCII when Shift is pressed
    {
        /* 0x00 */ NON_PRINTABLE,
        /* 0x01 */ NON_PRINTABLE,
        /* 0x02 */ '!',  // shift + 1 = !
        /* 0x03 */ '@',  // shift + 2 = @
        /* 0x04 */ '#',  // shift + 3 = #
        /* 0x05 */ '$',  // shift + 4 = $
        /* 0x06 */ '%',  // shift + 5 = %
        /* 0x07 */ '^',  // shift + 6 = ^
        /* 0x08 */ '&',  // shift + 7 = &
        /* 0x09 */ '*',  // shift + 8 = *
        /* 0x0A */ '(',  // shift + 9 = (
        /* 0x0B */ ')',  // shift + 0 = )
        /* 0x0C */ '_',  // shift + - = _
        /* 0x0D */ '+',  // shift + = = +
        /* 0x0E */ '\b', // Backspace (same as without Shift)
        /* 0x0F */ '\t',
        /* 0x10 */ 'Q',
        /* 0x11 */ 'W',
        /* 0x12 */ 'E',
        /* 0x13 */ 'R',
        /* 0x14 */ 'T',
        /* 0x15 */ 'Y',
        /* 0x16 */ 'U',
        /* 0x17 */ 'I',
        /* 0x18 */ 'O',
        /* 0x19 */ 'P',
        /* 0x1A */ '{',
        /* 0x1B */ '}',
        /* 0x1C */ '\n',
        /* 0x1D */ NON_PRINTABLE,
        /* 0x1E */ 'A',
        /* 0x1F */ 'S',
        /* 0x20 */ 'D',
        /* 0x21 */ 'F',
        /* 0x22 */ 'G',
        /* 0x23 */ 'H',
        /* 0x24 */ 'J',
        /* 0x25 */ 'K',
        /* 0x26 */ 'L',
        /* 0x27 */ ':',
        /* 0x28 */ '\"',
        /* 0x29 */ '~',
        /* 0x2A */ NON_PRINTABLE,
        /* 0x2B */ '|',
        /* 0x2C */ 'Z',
        /* 0x2D */ 'X',
        /* 0x2E */ 'C',
        /* 0x2F */ 'V',
        /* 0x30 */ 'B',
        /* 0x31 */ 'N',
        /* 0x32 */ 'M',
        /* 0x33 */ '<',
        /* 0x34 */ '>', // NO funciona
        /* 0x35 */ '?'  // NO funciona
    },
};

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
