#include <colors.h>
#include <drivers/keyboard_driver.h>
#include <drivers/time.h>
#include <naiveConsole.h>
#include <uint.h>
#include <stdlib.h>
#include <string.h>

void int_20(void);
void int_21(void);
// static void kbd_handler();

typedef void (*interruption_signature)(void);

static interruption_signature ints[] = {int_20, int_21};

void irqDispatcher(uint64_t irq) {
        ints[irq]();
}

void int_20(void) {
        // ncPrintOld("TIMERTICK ");
        timer_handler();
}

void int_21(void) {
        uint8_t c = keyboard_handler();

        if (c != 0) {
                // ncPrintCharText(c, BLACK_WHITE);
                update_buffer(c);
        }
}
