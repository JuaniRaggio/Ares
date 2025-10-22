#include <naiveConsole.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <keyboard.h>

static void int_20();
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
        //ncPrint("TIMERTICK ");
        timer_handler();
}

void int_21() { 
        char tecla = keyboard_handler();
        if (tecla != 0) {
                ncPrintCharNoColor(tecla);
        }
}
