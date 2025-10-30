#include <configuration.h>
#include <parser.h>
#include <shell.h>

#include <drivers/keyboard_driver.h>
#include <drivers/video_driver.h>
#include <stdbool.h>
#include <stdint.h>

#define DEFAULT_PROMPT_S 100
#define DEFAULT_HISTORY_S 10
#define PROMPT_SIZE DEFAULT_PROMPT_S
#define HISTORY_SIZE DEFAULT_HISTORY_S
#define SCREEN_SIZE (TEXT_WIDTH * TEXT_HEIGHT)

#define ever (;;)

static const char *const input_prompt = " > ";

static uint8_t prompt[PROMPT_SIZE] = {0};

static uint8_t lastest_prompt                            = 0;
static uint8_t prompt_history[HISTORY_SIZE][PROMPT_SIZE] = {0};

typedef struct {
        cursor_shape shape;
        uint8_t x, y;
        // uint8_t head, tail;
        uint8_t focused;
} shell_cursor;

static shell_cursor cursor = {
    .shape   = underline,
    .x       = 0,
    .y       = 0,
    .focused = true,
};

void welcome_shell() {
        // Imprimir un mensaje en pantalla que diga ARES
        return;
}

void show_input_prompt() {
        for (int i = 0; input_prompt[i] != '\0'; ++i, ++cursor.x) {
                drawChar(input_prompt[i], cursor.x, cursor.y, background_color,
                         user_font);
        }
        cursor.y++;
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
        for
                ever {
                        if (buffer_has_next()) {
                                character = buffer_next();
                                drawChar(character, cursor.x++, cursor.y,
                                         font_color, user_font);
                                prompt[i++] = character;
                                if (character == '\n') {
                                        cursor.y++;
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

void shell(void) {
        welcome_shell();
        init_shell();
        shell_loop();
        // return 0;
}
