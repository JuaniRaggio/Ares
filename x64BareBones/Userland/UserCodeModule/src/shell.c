#include <commands.h>
#include <configuration.h>
#include <lib.h>
#include <shell.h>
#include <stdint.h>
#include <stdio.h>
#include <syscall.h>

#define TRUE 1
#define FALSE 0
#define for_ever for (;;)
#define MAX_CHARS 256
#define CHECK_MAN "Type \"man %s\" to see how the command works\n"

static const char *const welcome_msg_shell =
    "Ares Recursive Experimental System\n";
static const char *const input_prompt = " > ";

static uint8_t lastest_prompt_idx();
static void add_to_history(const char *command);

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

static uint8_t lastest_prompt_idx() {
        return shell_status.prompts.lastest_prompt_idx;
}

static void add_to_history(const char *command) {
        if (command[0] == 0 || command[0] == '\n')
                return;
        strncpy(shell_status.prompts.prompt_history[lastest_prompt_idx()],
                command, PROMPT_SIZE - 1);
        shell_status.prompts
            .prompt_history[lastest_prompt_idx()][PROMPT_SIZE - 1] = 0;
        shell_status.prompts.lastest_prompt_idx =
            (lastest_prompt_idx() + 1) % HISTORY_SIZE;
}

void run_shell(void) {
        printf(welcome_msg_shell);
        printf("Type 'help' to see available commands\n\n");
        for_ever {
                printf(input_prompt);
                char command[MAX_CHARS] = {0};
                char arg1[MAX_CHARS]    = {0};
                char arg2[MAX_CHARS]    = {0};
                int qty_params = scanf("%s %s %s", command, arg1, arg2);
                if (command[0] == 0)
                        continue;
                add_to_history(command);

                if (!strcmp(command, "exit")) {
                        printf("Exiting Ares OS...\n");
                        syscall_exit(0);
                }

                int index = get_command_index(command);

                if (index == -1) {
                        printf(invalid_command);
                        continue;
                }

                int func_params = commands[index].ftype;
                if (qty_params - 1 != func_params) {
                        printf(wrong_params);
                        printf(CHECK_MAN, command);
                        continue;
                }

                switch (commands[index].ftype) {
                case supplier:
                        commands[index].f();
                        break;
                case function:
                        commands[index].g(arg1);
                        break;
                case bi_function:
                        commands[index].h(arg1, arg2);
                        break;
                }
        }
}

int shell(void) {
        run_shell();
        // Posible error as value
        return 0;
}
