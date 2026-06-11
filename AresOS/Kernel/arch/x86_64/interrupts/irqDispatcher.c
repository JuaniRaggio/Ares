#include <colors.h>
#include <drivers/keyboard_driver.h>
#include <drivers/time.h>
#include <naiveConsole.h>
#include <process.h>

void int_20(uint64_t *stack_ptr);
void int_21(uint64_t *stack_ptr);

typedef void (*interruption_signature)(uint64_t *stack_ptr);

static interruption_signature ints[] = {
    int_20,
    int_21,
};

void irqDispatcher(uint64_t irq, uint64_t *stack_ptr) {
        ints[irq](stack_ptr);
}

void int_20(uint64_t *stack_ptr) {
        timer_handler();
}

void int_21(uint64_t *stack_ptr) {
        uint8_t c = keyboard_handler(stack_ptr);

        if (c == CTRL_C_CHAR) {
                if (process_kill_foreground() > 0) {
                        ncPrint("^C\n", VGA_WHITE);
                }
                return;
        }

        if (c == CTRL_D_CHAR) {
                buffer_set_eof();
                process_wake_keyboard_readers();
                return;
        }

        if (c != 0) {
                update_buffer(c);
                process_wake_keyboard_readers();
        }
}
