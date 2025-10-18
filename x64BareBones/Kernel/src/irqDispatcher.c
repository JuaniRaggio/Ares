#include <naiveConsole.h>
#include <stdint.h>
#include <time.h>

static void int_20();

void irqDispatcher(uint64_t irq) {
        switch (irq) {
        case 0:
                int_20();
                break;
        }
        return;
}

void int_20() {
        ncPrint("TIMERTICK ");
        timer_handler();
}
