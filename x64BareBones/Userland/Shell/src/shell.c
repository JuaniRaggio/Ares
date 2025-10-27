#include <configuration.h>
#include <keyboard_driver.h>
#include <shell.h>
#include <stdbool.h>
#include <stdint.h>
#include <video_driver.h>

static const uint64_t screen_size     = TEXT_WIDTH * TEXT_HEIGHT;
static const char *const input_prompt = " > ";

typedef struct {
        cursor_shape shape;
        uint8_t x, y;
        uint8_t focused;
} shell_cursor;

static shell_cursor cursor = {
    .shape   = block,
    .x       = 0,
    .y       = 0,
    .focused = true,
};

void welcome_shell() {
        return;
}

void show_prompt() {
        for (int i = 0; input_prompt[i] != '\0'; ++i) {
                drawChar(input_prompt[i], cursor.x++, cursor.y, background_color, user_font);
        }
        cursor.y++;
}

// Read from keyboard driver
// Save prompt (from last input prompt till enter)
// Parse prompt

void shell_loop() {
        for (;;) {
                if (buffer_has_next()) {
                        uint8_t character = buffer_next();
                        drawChar(character, cursor.x++, cursor.y, font_color);
                }
        }
}

void init_shell() {
}

int shell(void) {
        uint8_t buffer[screen_size] = {0};
        welcome_shell();
        init_shell();
        shell_loop();
        return 0;
}
