#include <configuration.h>
#include <keyboard_driver.h>
#include <parser.h>
#include <shell.h>
#include <stdbool.h>
#include <stdint.h>
#include <video_driver.h>

#define DEFAULT_PROMPT_S 100
#define DEFAULT_HISTORY_S 10

static const uint64_t screen_size        = TEXT_WIDTH * TEXT_HEIGHT;
static const uint8_t *const input_prompt = " > ";

static const uint8_t prompt_size   = DEFAULT_PROMPT_S;
static uint8_t prompt[prompt_size] = {0};

static const uint8_t history_size                        = DEFAULT_HISTORY_S;
static uint8_t lastest_prompt                            = 0;
static uint8_t prompt_history[history_size][prompt_size] = {0};

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
        for (uint8_t character, i = 0;;) {
                if (buffer_has_next()) {
                        character = buffer_next();
                        drawChar(character, cursor.x++, cursor.y, font_color,
                                 user_font);
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
}

static uint8_t buffer[screen_size] = {0};

int shell(void) {
        welcome_shell();
        init_shell();
        shell_loop();
        return 0;
}
