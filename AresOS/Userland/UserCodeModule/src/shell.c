#include <commands.h>
#include <stdint.h>
#include <stdio.h>

#define TRUE 1
#define FALSE !TRUE
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
        char user_input[max_parameters][MAX_CHARS];
        composed_command_t prompt_history[HISTORY_SIZE];
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
        {
            .shape   = underline,
            .x       = 0,
            .y       = 0,
            .focused = TRUE,
        },
};

static uint8_t lastest_prompt_idx() {
        return shell_status.prompts.lastest_prompt_idx;
}

#define INVALID_INPUT 0
#define VALID_INPUT 1

uint8_t analize_user_input(uint32_t params) {
        int idx = get_command_index(shell_status.prompts.user_input[0]);
        if (idx == INVALID_COMMAND_NAME) {
                printf(invalid_command);
                return INVALID_COMMAND_NAME;
        } else if (commands[idx]->lambda.ftype != params - 1) {
                printf(wrong_params);
                printf(CHECK_MAN, shell_status.prompts.user_input[0]);
                return INVALID_INPUT;
        }
        shell_status.prompts.prompt_history[lastest_prompt_idx()].cmd =
            commands[idx];
        return VALID_INPUT;
}

static char * helper_msg = "Type 'help' to see available commands\n\n";

int shell(void) {
        clear_cmd();
        printf(welcome_msg_shell);
        printf(helper_msg);
        for_ever {
                printf(input_prompt);
                int r_arguments =
                    scanf("%s %s %s", shell_status.prompts.user_input[0],
                          shell_status.prompts.user_input[1],
                          shell_status.prompts.user_input[2]);
                if (shell_status.prompts.user_input[0][0] == 0)
                        continue;

                analize_user_input(r_arguments);
                composed_command_t current_prompt =
                    shell_status.prompts.prompt_history[lastest_prompt_idx()];
                executable_t execute = current_prompt.cmd->lambda.execute;
                switch (current_prompt.cmd->lambda.ftype) {
                case supplier_t:
                        execute.supplier();
                        break;
                case function_t:
                        execute.function(current_prompt.args[0]);
                        break;
                case bi_function_t:
                        execute.bi_function(current_prompt.args[0],
                                            current_prompt.args[1]);
                        break;
                }
        }
        return 0;
}
