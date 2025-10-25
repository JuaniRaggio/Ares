#include <drivers/keyboard_driver.h>
#include <stdint.h>

typedef struct {
        uint8_t buffer[256];
        uint8_t write_pos; //head
        uint8_t read_pos;  //tail 
        uint8_t modifiers; // Shift, Ctrl, Alt
} keyboard_state_t;

static keyboard_state_t keyboard = {0};

#define LSHIFT_CODE 0x2A
#define RSHIFT_CODE 0x36
#define BREAK_CODE 0x80

uint8_t keyboard_handler() {
        uint8_t scan_code = get_input();

        // BREAK_CODE -> Se suelta tecla
        if (scan_code & BREAK_CODE) {
                uint8_t make_code = scan_code & 0x7F; // le saco el bit 7

                if (make_code == LSHIFT_CODE || make_code == RSHIFT_CODE) {
                        keyboard.modifiers = (uint8_t)off; // se solto el shift
                }
                goto end;
        }
        if (scan_code == LSHIFT_CODE || scan_code == RSHIFT_CODE) {
                keyboard.modifiers = (uint8_t)shift; // se presionó Shift
                goto end;
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
        if(!buffer_has_next) {
                return 0; //No char in buffer
        }

        uint8_t aux = keyboard.buffer[keyboard.read_pos];

        keyboard.read_pos = (keyboard.read_pos+1) & 0xff;

        return aux;
}