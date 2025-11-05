#include <colors.h>
#include <drivers/keyboard_driver.h>
#include <drivers/time.h>
#include <naiveConsole.h>
#include <regs.h>

extern regs_snapshot_t saved_regs;

void int_20(uint64_t *stack_ptr);
void int_21(uint64_t *stack_ptr);

typedef void (*interruption_signature)(uint64_t *stack_ptr);

static interruption_signature ints[] = {
    int_20,
    int_21,
};

static void save_register_state(uint64_t *stack_frame) {
        // Guardar todos los registros del stack frame
        saved_regs.r15    = stack_frame[0];
        saved_regs.r14    = stack_frame[1];
        saved_regs.r13    = stack_frame[2];
        saved_regs.r12    = stack_frame[3];
        saved_regs.r11    = stack_frame[4];
        saved_regs.r10    = stack_frame[5];
        saved_regs.r9     = stack_frame[6];
        saved_regs.r8     = stack_frame[7];
        saved_regs.rsi    = stack_frame[8];
        saved_regs.rdi    = stack_frame[9];
        saved_regs.rbp    = stack_frame[10];
        saved_regs.rdx    = stack_frame[11];
        saved_regs.rcx    = stack_frame[12];
        saved_regs.rbx    = stack_frame[13];
        saved_regs.rax    = stack_frame[14];
        saved_regs.rip    = stack_frame[15];
        saved_regs.cs     = stack_frame[16];
        saved_regs.rflags = stack_frame[17];
        saved_regs.rsp    = stack_frame[18];
        saved_regs.ss     = stack_frame[19];
}

void irqDispatcher(uint64_t irq, uint64_t *stack_ptr) {
        ints[irq](stack_ptr);
}

void int_20(uint64_t *stack_ptr) {
        timer_handler();
}

void int_21(uint64_t *stack_ptr) {
        uint8_t c = keyboard_handler(stack_ptr);

        if (c != 0) {
                // Guardar estado de registros cuando se presiona una tecla
                save_register_state(stack_ptr);
                update_buffer(c);
        }
}
