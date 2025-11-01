#include "syscallDispatcher.h"
#include <configuration.h>
#include <math.h>
#include <parser.h>
#include <shell.h>

#include <drivers/keyboard_driver.h>
#include <drivers/video_driver.h>
#include <stdbool.h>
#include <uint.h>

#define DEFAULT_PROMPT_S 100
#define DEFAULT_HISTORY_S 10
#define PROMPT_SIZE DEFAULT_PROMPT_S
#define HISTORY_SIZE DEFAULT_HISTORY_S
#define SCREEN_SIZE (TEXT_WIDTH * TEXT_HEIGHT)
#define for_ever for (;;)

static const char *const welcome_msg_shell = "Welcome back!\n";
static const char *const input_prompt      = " > ";

typedef struct {
        cursor_shape shape;
        uint8_t x, y;
        uint8_t head, tail;
        uint8_t focused;
} shell_cursor;

typedef struct {
        uint8_t lastest_prompt_idx;
        char prompt[PROMPT_SIZE];
        char prompt_history[HISTORY_SIZE][PROMPT_SIZE];
} prompt_data;

typedef struct {
        float magnification;
        float font_size;
        prompt_data prompts;
        shell_cursor cursor;
} shell_attributes;

static shell_attributes shell_status = {
    .magnification = 1,
    .prompts       = (prompt_data){0},
    .cursor =
        (shell_cursor){
            .shape   = underline,
            .x       = 0,
            .y       = 0,
            .focused = true,
        },
};

void welcome_shell() {
        // shell_printf(welcome_msg_shell);
        return;
}

// void shell_printf(const char *msg) {
// }

// struct que deberia estar en asm
typedef struct {
        uint64_t rip;
        // Aca irian todos los registros
        // uint64_t
        // uint64_t
        // uint64_t
        // uint64_t
        // uint64_t
} regs;

extern regs get_register_values();

static void print_registers(void) {
        static const char *const regNames[] = {
            "RIP", "RSP", "RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI",
            "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15"};
        // Si o si llamariamos a una funcion de .asm
        // inline __asm__ {
        //         pushstate (En orden en el que se deseen los registros)
        //         call syswrite
        // }
}

void show_input_prompt() {
        for (int i = 0; input_prompt[i] != '\0'; ++i, ++shell_status.cursor.x) {
                drawChar(input_prompt[i], shell_status.cursor.x,
                         shell_status.cursor.y, background_color, user_font);
        }
        shell_status.cursor.y++;
}

// Read from keyboard driver
// Save prompt (from last input prompt till enter)
// Parse prompt

void save_prompt() {
        for (int i = 0; prompt[i] != 0; ++i) {
                prompt_history[lastest_prompt][i] = prompt[i];
        }
        ++lastest_prompt;
}

void shell_loop() {
        uint8_t character, i = 0;
        for_ever {
                if (buffer_has_next()) {
                        character = buffer_next();
                        drawChar(character, shell_status.cursor.x++,
                                 shell_status.cursor.y, font_color, user_font);
                        prompt[i++] = character;
                        if (character == '\n') {
                                shell_status.cursor.y++;
                                if (analize_prompt(prompt))
                                        save_prompt();
                                i = 0;
                                show_input_prompt();
                        }
                }
        }
}

void init_shell() {
        // TODO
}

static uint8_t buffer[SCREEN_SIZE] = {0};

int shell(void) {
        welcome_shell();
        init_shell();
        shell_loop();
        return 0;
}
