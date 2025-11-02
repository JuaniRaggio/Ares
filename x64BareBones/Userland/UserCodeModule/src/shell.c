#include <configuration.h>
#include <parser.h>
#include <shell.h>
#include <syscall.h>

#include <drivers/keyboard_driver.h>
#include <drivers/video_driver.h>

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
        char buffer[SCREEN_SIZE];
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

//======================================
// para testear que funca, dsp borrar ;)
void shell_main() {
        const char *msg = "Hola desde la shell!\n";
        syscall_write(1, msg, 21);
}
//======================================

void welcome_shell() {
        // shell_printf(welcome_msg_shell);
        return;
}

// void shell_printf(const char *msg) {
// }

static void print_registers(void) {
        static const char *const reg_names[] = {
            "RIP", "RSP", "RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI",
            "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15"};
        void *snapshot = get_register_values();
        for (int i = 0; reg_names[i]; i++) {
                // Aca habria que hacer un print de reg_names[i] por un lado
                // y de (uint64_t)(snapshot + 8*i) pero en hexa
                // La otra opcion seria que el codigo de assembly haga los
                // prints directamente
        }
}

void show_input_prompt() {
        // ===== TODO Esto deberiamos cambiarlo por un printf =====
        for (int i = 0; input_prompt[i] != '\0'; ++i, ++shell_status.cursor.x) {
                drawChar(input_prompt[i], get_x_cursor(), get_y_cursor(),
                         background_color, user_font);
        }
        shell_status.cursor.y++;
}

uint8_t get_y_cursor() {
        return shell_status.cursor.y;
}

uint8_t get_x_cursor() {
        return shell_status.cursor.x;
}

uint8_t lastest_prompt_idx() {
        return shell_status.prompts.lastest_prompt_idx;
}

char *current_prompt() {
        return shell_status.prompts.prompt;
}

void save_prompt() {
        for (int i = 0; current_prompt()[i] != 0; ++i) {
                shell_status.prompts.prompt_history[lastest_prompt_idx()][i] =
                    shell_status.prompts.prompt[i];
        }
        ++shell_status.prompts.lastest_prompt_idx;
}

void shell_loop() {
        uint8_t character, i = 0;
        for_ever {
                if (buffer_has_next()) {
                        character = buffer_next();
                        drawChar(character, shell_status.cursor.x++,
                                 get_y_cursor(), font_color, user_font);
                        current_prompt()[i++] = character;
                        // Solucion facil para ejecutar una vez se llega
                        // al final
                        if (character == '\n' || i == PROMPT_SIZE - 1) {
                                shell_status.cursor.y++;
                                if (analize_prompt(current_prompt()))
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

int shell(void) {
        welcome_shell();
        init_shell();
        shell_loop();
        return 0;
}
