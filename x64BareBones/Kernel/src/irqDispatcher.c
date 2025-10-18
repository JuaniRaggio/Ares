#include <naiveConsole.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void int_20();

typedef void (*interruption_signature(void));

inline void irqDispatcher(uint64_t irq, void * algo) {
        static interruption_signature interruption_routines[];
        interruption_routines[20] = int_20;
        return;
}

void int_20() {
        ncPrint("TIMERTICK ");
        timer_handler();
}
