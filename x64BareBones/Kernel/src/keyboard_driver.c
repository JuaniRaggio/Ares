#include <keyboard_driver.h>
#include <stdint.h>

typedef struct {
        uint8_t buffer[256];
        uint8_t read_pos;
        uint8_t write_pos;
        uint8_t modifiers; // Shift, Ctrl, Alt
} keyboard_state_t;

static keyboard_state_t keyboard = {0};

static uint8_t shift_pressed = 0;

#define LSHIFT_CODE 0x2A
#define RSHIFT_CODE 0x36
#define BREAK_CODE 0x80

char keyboard_handler() {
        uint8_t scan_code = get_input();

        // BREAK_CODE -> Se suelta tecla
        if (scan_code & BREAK_CODE) {
                uint8_t make_code = scan_code & 0x7F; // le saco el bit 7

                if (make_code == LSHIFT_CODE || make_code == RSHIFT_CODE) {
                        keyboard.modifiers = (uint8_t)off;
                }

                return 0; // no imprimimos nada al soltar teclas
        } else if (scan_code == LSHIFT_CODE || scan_code == RSHIFT_CODE) {
                keyboard.modifiers = (uint8_t)shift; // se presionó Shift
                return 0;
        }

        char c;

        // Convertir scancode a carácter
        if (shift_pressed) {
                c = ascii_shift_table[scan_code];
        } else {
                c = ascii_table[scan_code];
        }

        return c;
}
