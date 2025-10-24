#include <keyboard.h>

static int shift_pressed = 0;

char keyboard_handler() {
    uint8_t scan_code = get_input();
    
    // Si es un break code (tecla soltada)
    if (scan_code & 0x80) {
        uint8_t make_code = scan_code & 0x7F; // le saco el bit 7

        if (make_code == 0x2A || make_code == 0x36) {
            shift_pressed = 0; // se soltó Shift
        }

        return 0; // no imprimimos nada al soltar teclas
    }

    // Si es un make code (tecla presionada)
    if (scan_code == 0x2A || scan_code == 0x36) {
        shift_pressed = 1; // se presionó Shift
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

//TODO:


// void update_screen() {
//     for (int i = 0; i < keyboard->read_pos-1; i++) {
//         ncPrintChar(keyboard->buffer[i], BLACK_WHITE);
//     }
// }

// void init_keyboard() {
//     for(int i = 0; i < 256; i++) {
//         keyboard->buffer[i] = 0;
//     }
//     keyboard->read_pos = 0;
//     keyboard->shift = false;
// }
