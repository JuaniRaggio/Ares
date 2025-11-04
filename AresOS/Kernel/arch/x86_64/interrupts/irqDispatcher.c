#include <colors.h>
#include <drivers/keyboard_driver.h>
#include <drivers/time.h>
#include <naiveConsole.h>

void int_20(uint64_t *stack_ptr);
void int_21(uint64_t *stack_ptr);
void kbd_handler();

typedef void (*interruption_signature)(uint64_t *stack_ptr);

static interruption_signature ints[] = {int_20, int_21};

void irqDispatcher(uint64_t irq, uint64_t *stack_ptr) {
        ints[irq](stack_ptr);
}

void int_20(uint64_t *stack_ptr) {
        timer_handler();
}

void int_21(uint64_t *stack_ptr) {
        uint8_t c = keyboard_handler(stack_ptr);

        if (c != 0) {
                update_buffer(c);
        }
}
