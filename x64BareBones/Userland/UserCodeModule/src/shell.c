#include <configuration.h>
#include <parser.h>
#include <shell.h>
#include <stdint.h>
#include <stdio.h>
#include <syscall.h>

#define TRUE 1
#define FALSE 0
#define for_ever for (;;)

static const char *const welcome_msg_shell = "Welcome back!\n";
static const char *const input_prompt      = " > ";

// @return: input_prompt length, this is inlined because it should be resolved
// in compile time since input_prompt won't change at runtime and its also a
// static variable
static inline uint8_t input_prompt_length() {
        int length = 0;
        while (input_prompt[length])
                length++;
        return length;
}

// @return: pointer to the current prompt, this means, the prompt
// attribute inside shell_status
static char *current_prompt();

// @return: index in history of the lastest executed prompt
static uint8_t lastest_prompt_idx();

// @return: the horizontal number of cell which the cursor should be in
static uint8_t get_x_cursor();

// @return: the vertical number of cell which the cursor should be in
static uint8_t get_y_cursor();

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
            .focused = TRUE,
        },
};

static uint8_t get_y_cursor() {
        return shell_status.cursor.y;
}
static uint8_t get_x_cursor() {
        return shell_status.cursor.x;
}
static uint8_t lastest_prompt_idx() {
        return shell_status.prompts.lastest_prompt_idx;
}

static char *current_prompt() {
        return shell_status.prompts.prompt;
}

void print_registers(void) {
        static const uint8_t reg_size_B      = 8;
        static const char *const reg_names[] = {
            "RIP", "RSP", "RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI",
            "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15",
        };
        void *snapshot = get_register_values();
        for (int i = 0; reg_names[i]; i++) {
                printf("%s: 0x%x\n", reg_names[i],
                       (uint64_t *)(snapshot + i * reg_size_B));
        }
}

void welcome_shell() {
        syscall_write(1, welcome_msg_shell, 14);
        return;
}

void show_input_prompt() {
        printf("%s", input_prompt);
        shell_status.cursor.x = 3;
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
                        // Actualizar cursor de la shell
                        shell_status.cursor.x++;
                        // Escribir el caracter usando syscall
                        syscall_write(1, (const char *)&character, 1);
                        current_prompt()[i++] = character;
                        // Solucion facil para ejecutar una vez se llega
                        // al final
                        if (character == '\n' || i == PROMPT_SIZE - 1) {
                                shell_status.cursor.y++;
                                if (analize_prompt(current_prompt()))
                                        save_prompt();
                                i = 0;
                                // Mostrar prompt usando syscall
                                syscall_write(1, input_prompt, 3); // " > "
                        }
                }
        }
}

int shell(void) {
        welcome_shell();
        shell_loop();
        return 0;
}
