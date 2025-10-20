#include <naiveConsole.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void int_20();
// static void kbd_handler();

typedef void (*interruption_signature)(void);

void irqDispatcher(uint64_t irq) {
        
        interruption_signature interruption_routines[256];
        // = {
        //         timer_handler,
        //         // kbd_handler,
        // };
        interruption_routines[20] = timer_handler;
        return;
}

void int_20() {
        ncPrint("TIMERTICK ");
        timer_handler();
}
