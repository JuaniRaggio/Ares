#include "configuration.h"
#include "lib.h"
#include <commands.h>
#include <shell.h>
#include <stdint.h>
#include <stdio.h>
#include <syscalls.h>

#define TRUE 1
#define FALSE !TRUE
#define for_ever for (;;)
#define MAX_CHARS 256
#define CHECK_MAN "Type \"man %s\" to see how the command works\n"
#define PROMPT_LENGTH 3
#define CURSOR_BLINK_TICKS 25
#define MAX_PARAMS 3

static const char *const helper_msg =
    "Type 'help' to see available commands\n\n";
static const char *const welcome_msg_shell =
    "Ares Recursive Experimental System\n MMMMm                 MMMMMMMMMMM    "
    "               mMMMMMMMMMMMm\n MMMMMMM               mMMMMMMMMMm          "
    "       MMMMMMMMMMM\n MMMMMMMMm              MMMMMMMMM                "
    "mMMMMMMMMMm\n MMMMMMMMM            mMMMMMMMm              MMMMMMMMM\n "
    "mMMMMMMMMm           MMMMMMM             mMMMMMMMMm\n MMMMMMMM         "
    "mMMMMMm           MMMMMMMM         0\n mMMMMMm         MMMMM           "
    "mMMMMMm         o@@@o              mmm\n MMMMM       mMMMm         MMMMM  "
    "          o@@@@@@o     mmmmmMMMMM\n mMMMm       MMM         mMMMm         "
    "   o@@@@@@@@@oMMMMMMMMMMMMM\n @oo              MMM      mMm        MMM    "
    "         o@@@@@@@@@@@@@@ooMMMMMMMMM\n @@@@o              mMm     M       "
    "mMm              o@@@@@@@@@@@@@@@@@@@oooMMMM\n @@@@@@@@o           "
    "MhhhHHHHHHHHhhhM              o@@@@@@@@@@@@@@@@@@@@@@@@@oo\n @@@@@@@@@o   "
    "    hhHHHHHHHHHHHHHHHHHhh       oo@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n "
    "@@@@@@@@@@@oo hHHHHHHHHHHHHHHHHHHHHHHHHh "
    "oo@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n "
    "@@@@@@@@@@@@@@ooHHHHHHHHHHHHHHHHHHHHoo@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    "@@@@@\n "
    "@@@@@@@@@@@@@@@@ooHHHHHHHHHHHHHHHHoo@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    "@@@@@\n "
    "@@@@@@@@@@@@@@@@@@@ooHHHHHHHHHoo@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    "@@@@@\n "
    "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@AresSystem@@@"
    "@@@@@\n";
static const char *const input_prompt = " > ";
static s_time start_time;

static void sync_cursor_pos(void);
static uint8_t lastest_prompt_idx();
static void add_to_history(const command_t *command, uint32_t params);

typedef struct {
        cursor_shape shape;
        uint32_t color;
        uint8_t border_width;
        uint8_t line_width;
        uint8_t underline_height;
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
        uint8_t magnification;
        float font_size;
        uint32_t font_color;
        uint32_t background_color;
        prompt_data prompts;
        shell_cursor cursor;
} shell_attributes;

shell_attributes shell_status = {
    .magnification    = MIN_FONT_SCALE,
    .prompts          = (prompt_data){0},
    .font_color       = default_font_color,
    .background_color = default_background_color,
    .cursor =
        {
            .shape            = underline,
            .color            = WHITE,
            .border_width     = 1,
            .line_width       = 2,
            .underline_height = 2,
            .x                = 0,
            .y                = 0,
            .focused          = TRUE,
        },
};

static uint8_t lastest_prompt_idx() {
        return shell_status.prompts.lastest_prompt_idx;
}

#define INVALID_INPUT 0
#define VALID_INPUT 1

static void add_to_history(const command_t *command, uint32_t params) {
        shell_status.prompts.prompt_history[lastest_prompt_idx()].cmd = command;
        int i;
        for (uint32_t p = 1; p < params; p++) {
                for (i = 0;
                     shell_status.prompts.user_input[p][i] && i < MAX_CHARS - 1;
                     i++) {
                        shell_status.prompts
                            .prompt_history[lastest_prompt_idx()]
                            .args[p - 1][i] =
                            shell_status.prompts.user_input[p][i];
                }
                shell_status.prompts.prompt_history[lastest_prompt_idx()]
                    .args[p - 1][i] = '\0';
        }
}

static void draw_cursor(uint8_t x, uint8_t y, uint8_t visible) {
        if (!visible)
                return;

        uint8_t scale = shell_status.magnification;

        uint16_t px           = x * FONT_WIDTH * scale;
        uint16_t py           = y * FONT_HEIGHT * scale;
        uint32_t cursor_color = WHITE;

        uint8_t border_width     = shell_status.cursor.border_width * scale;
        uint8_t line_width       = shell_status.cursor.line_width * scale;
        uint8_t underline_height = shell_status.cursor.underline_height * scale;

        switch (shell_status.cursor.shape) {
        case block:
                syscall_draw_rect(px, py, FONT_WIDTH * scale,
                                  FONT_HEIGHT * scale, cursor_color);
                break;
        case hollow:
                syscall_draw_rect(px, py, FONT_WIDTH * scale, border_width,
                                  cursor_color);
                syscall_draw_rect(px, py + FONT_HEIGHT * scale - border_width,
                                  FONT_WIDTH * scale, border_width,
                                  cursor_color);
                syscall_draw_rect(px, py, border_width, FONT_HEIGHT * scale,
                                  cursor_color);
                syscall_draw_rect(px + FONT_WIDTH * scale - border_width, py,
                                  border_width, FONT_HEIGHT * scale,
                                  cursor_color);
                break;
        case line:
                syscall_draw_rect(px, py, line_width, FONT_HEIGHT * scale,
                                  cursor_color);
                break;
        case underline:
                syscall_draw_rect(
                    px, py + FONT_HEIGHT * scale - underline_height,
                    FONT_WIDTH * scale, underline_height, cursor_color);
                break;
        }
}

static void erase_cursor(int x, int y) {
        uint8_t scale = shell_status.magnification;

        int px = x * FONT_WIDTH * scale;
        int py = y * FONT_HEIGHT * scale;
        syscall_draw_rect(px, py, FONT_WIDTH * scale, FONT_HEIGHT * scale,
                          BLACK);
}

int shell_read_line(char input[][256], int max_params) {
        char buffer[MAX_CHARS];
        int buf_idx       = 0;
        int param_count   = 0;
        int current_param = 0;

        uint64_t last_blink = syscall_get_ticks();
        int cursor_visible  = 0;

        for (int i = 0; i < max_params; i++) {
                input[i][0] = 0;
        }

        for_ever {
                char c = getchar();

                if (c == 0) {
                        uint64_t now = syscall_get_ticks();
                        if (now - last_blink > CURSOR_BLINK_TICKS &&
                            !cursor_visible) {
                                draw_cursor(shell_status.cursor.x,
                                            shell_status.cursor.y, 1);
                                cursor_visible = 1;
                                last_blink     = now;
                        }
                        continue;
                }

                if (c == ZOOM_IN_CHAR) {
                        if (shell_status.magnification < MAX_FONT_SCALE) {
                                shell_status.magnification++;
                                syscall_set_font_size(
                                    shell_status.magnification);
                                syscall_redraw_screen();
                                sync_cursor_pos();
                        }
                        continue;
                }

                if (c == ZOOM_OUT_CHAR) {
                        if (shell_status.magnification > MIN_FONT_SCALE) {
                                shell_status.magnification--;
                                syscall_set_font_size(
                                    shell_status.magnification);
                                syscall_redraw_screen();
                                sync_cursor_pos();
                        }
                        continue;
                }

                if (c == '\n') {
                        if (buf_idx > 0) {
                                buffer[buf_idx] = 0;
                                int j           = 0;
                                for (int i = 0; i <= buf_idx && j < MAX_CHARS;
                                     i++, j++) {
                                        input[current_param][j] = buffer[i];
                                }
                                input[current_param][j] = 0;
                                param_count++;
                        }
                        putchar('\n');
                        sync_cursor_pos();
                        return param_count;
                }

                if (c == '\b') {
                        if (buf_idx > 0) {
                                buf_idx--;
                                putchar('\b');
                                sync_cursor_pos();
                                draw_cursor(shell_status.cursor.x,
                                            shell_status.cursor.y, 1);
                                cursor_visible = 1;
                                last_blink     = syscall_get_ticks();
                        }
                        continue;
                }

                if (c == ' ' && buf_idx > 0) {
                        buffer[buf_idx] = 0;
                        int j           = 0;
                        for (int i = 0; i <= buf_idx && j < MAX_CHARS;
                             i++, j++) {
                                input[current_param][j] = buffer[i];
                        }
                        input[current_param][j] = 0;
                        current_param++;
                        param_count++;
                        buf_idx = 0;

                        if (current_param >= max_params) {
                                while (getchar() != '\n')
                                        ;
                                putchar('\n');
                                sync_cursor_pos();
                                return param_count;
                        }
                        putchar(' ');
                        sync_cursor_pos();
                        draw_cursor(shell_status.cursor.x,
                                    shell_status.cursor.y, 1);
                        cursor_visible = 1;
                        last_blink     = syscall_get_ticks();
                        continue;
                }

                if (buf_idx < MAX_CHARS - 1) {
                        buffer[buf_idx++] = c;
                        putchar(c);
                        sync_cursor_pos();
                        draw_cursor(shell_status.cursor.x,
                                    shell_status.cursor.y, 1);
                        cursor_visible = 1;
                        last_blink     = syscall_get_ticks();
                }
        }
}

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
        add_to_history(commands[idx], params);
        return VALID_INPUT;
}

static void sync_cursor_pos(void) {
        int x, y;
        syscall_get_cursor_pos(&x, &y);
        shell_status.cursor.x = x;
        shell_status.cursor.y = y;
}

void init_shell(void) {
        clear_cmd();
        syscall_get_time(&start_time);
}

s_time get_shell_start_time(void) {
        return start_time;
}

int shell(void) {
        init_shell();
        printf(welcome_msg_shell);
        printf(helper_msg);
        sync_cursor_pos();

        for_ever {
                printf(input_prompt);
                sync_cursor_pos();
                char error      = VALID_INPUT;
                int r_arguments = shell_read_line(
                    shell_status.prompts.user_input, MAX_PARAMS);

                if (shell_status.prompts.user_input[0][0] == 0)
                        continue;

                error = analize_user_input(r_arguments);

                if (error == INVALID_INPUT || error == INVALID_COMMAND_NAME)
                        continue;

                composed_command_t current_prompt =
                    shell_status.prompts.prompt_history[lastest_prompt_idx()];
                executable_t execute = current_prompt.cmd->lambda.execute;
                switch (r_arguments - 1) {
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
