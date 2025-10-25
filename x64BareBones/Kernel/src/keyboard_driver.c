#include <keyboard_driver.h>

typedef struct {
    uint8_t buffer[256];
    uint8_t read_pos;
    uint8_t write_pos;
    uint8_t modifiers; // Shift, Ctrl, Alt
} keyboard_state_t;

keyboard_state_t keyboard = {0};

static uint8_t shift_pressed = 0;

char keyboard_handler() {
    uint8_t scan_code = get_input();
    
    // Si es un break code (tecla soltada)
    if (scan_code & 0x80) {
        uint8_t make_code = scan_code & 0x7F; // le saco el bit 7

        if (make_code == 0x2A || make_code == 0x36) {
            shift_pressed = (uint8_t) 0; // se soltó Shift
        }

        return 0; // no imprimimos nada al soltar teclas
    }

    // Si es un make code (tecla presionada)
    if (scan_code == 0x2A || scan_code == 0x36) {
        shift_pressed = (uint8_t) 1; // se presionó Shift
        return 0;
    }

    char c;

    // Convertir scancode a carácter
    if(shift_pressed) {
        c = ascii_shift_table[scan_code];
    } else {
        c = ascii_table[scan_code];
    }
    
    return c;

}



