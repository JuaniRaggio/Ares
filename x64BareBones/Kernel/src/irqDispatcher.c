#include <naiveConsole.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <keyboard_driver.h>
#include "../include/colors.h"


static void int_20();
void int_21();
// static void kbd_handler();

typedef void (*interruption_signature)(void);

void irqDispatcher(uint64_t irq) {
        if(irq == 0x00)
        {
                int_20();
        }
        if(irq == 0x01)
        {
                int_21();
        }
}

void int_20() {
        //ncPrintOld("TIMERTICK ");                                                                                                                                                                                                                                                                                                
        timer_handler();
}

void int_21() { 
        uint8_t c = keyboard_handler();
        
        //cheque que esta funcionando bien hasta aca
        if(c != 0) {
                update_buffer(c);
                //ncPrintChar(caracter, BLACK_WHITE);
        }
}
