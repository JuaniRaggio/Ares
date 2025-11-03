#include <configuration.h>
#include <lib.h>
#include <regs.h>
#include <shell.h>
#include <stdint.h>
#include <stdio.h>
#include <syscall.h>

#define TRUE 1
#define FALSE 0
#define for_ever for (;;)
#define MAX_CHARS 256
#define QTY_COMMANDS 9

static const char *const welcome_msg_shell = "=== Welcome to Ares OS ===\n";
static const char *const input_prompt      = " > ";
static const char *const invalid_command   = "Invalid command!\n";
static const char *const wrong_params      = "Invalid number of parameters\n";
static const char *const check_man =
    "Type \"man %s\" to see how the command works\n";

typedef enum { NO_PARAMS = 0, SINGLE_PARAM, DUAL_PARAM } function_type;

typedef struct {
        char *name;
        char *description;
        union {
                int (*f)(void);
                int (*g)(char *);
                int (*h)(char *, char *);
        };
        function_type ftype;
} command_t;

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

static command_t commands[QTY_COMMANDS];

static void help(void);
static void man(char *command);
static void print_info_reg(void);
static void show_time(void);
static int div_cmd(char *num, char *div);
static void clear_cmd(void);
static void print_mem(char *pos);
static void history_cmd(void);

static int get_command_index(char *command);
static void add_to_history(const char *command);
static void init_commands(void);

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

void init_shell() {
}

int shell(void) {
        init_shell();
        welcome_shell();
        shell_loop();
        return 0;
}
