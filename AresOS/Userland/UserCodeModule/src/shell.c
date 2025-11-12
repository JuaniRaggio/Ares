#include <commands.h>
#include <lib.h>
#include <shell.h>
#include <status_codes.h>
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
const char *const logo_shell = "    ___    ____  ______ _____ \n"
                               "   /   |  / __ \\/ ____// ___/ \n"
                               "  / /| | / /_/ / __/  /__ \\  \n"
                               " / ___ |/ _, _/ /___ ___/ /  \n"
                               "/_/  |_/_/ |_/_____//____/   \n\n";

static const char *const input_prompt = " > ";
static s_time start_time;

static void sync_cursor_pos(void);
static uint8_t lastest_prompt_idx();
static void add_to_history(const command_t *command, uint32_t params);

shell_attributes shell_status = {
    .magnification    = MIN_FONT_SCALE,
    .prompts          = (prompt_data){0},
    .font_height      = FONT_HEIGHT,
    .font_width       = FONT_WIDTH,
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

        uint16_t px           = x * shell_status.font_width * scale;
        uint16_t py           = y * shell_status.font_height * scale;
        uint32_t cursor_color = shell_status.cursor.color;

        uint8_t border_width     = shell_status.cursor.border_width * scale;
        uint8_t line_width       = shell_status.cursor.line_width * scale;
        uint8_t underline_height = shell_status.cursor.underline_height * scale;

        switch (shell_status.cursor.shape) {
        case block:
                syscall_draw_rect(px, py, shell_status.font_width * scale,
                                  shell_status.font_height * scale,
                                  cursor_color);
                break;
        case hollow:
                syscall_draw_rect(px, py, shell_status.font_width * scale,
                                  border_width, cursor_color);
                syscall_draw_rect(
                    px, py + shell_status.font_height * scale - border_width,
                    shell_status.font_width * scale, border_width,
                    cursor_color);
                syscall_draw_rect(px, py, border_width,
                                  shell_status.font_height * scale,
                                  cursor_color);
                syscall_draw_rect(
                    px + shell_status.font_width * scale - border_width, py,
                    border_width, shell_status.font_height * scale,
                    cursor_color);
                break;
        case line:
                syscall_draw_rect(px, py, line_width,
                                  shell_status.font_height * scale,
                                  cursor_color);
                break;
        case underline:
                syscall_draw_rect(px,
                                  py + shell_status.font_height * scale -
                                      underline_height,
                                  shell_status.font_width * scale,
                                  underline_height, cursor_color);
                break;
        }
}

static void erase_cursor(int x, int y) {
        uint8_t scale = shell_status.magnification;
        int px        = x * shell_status.font_width * scale;
        int py        = y * shell_status.font_height * scale;
        syscall_draw_rect(px, py, shell_status.font_width * scale,
                          shell_status.font_height * scale, BLACK);
}

int shell_read_line(char input[][MAX_CHARS], int max_params) {
        char buffer[MAX_CHARS];
        int buf_idx       = 0;
        int param_count   = 0;
        int current_param = 0;

        uint64_t last_blink = 0;
        syscall_get_ticks(&last_blink);
        int cursor_visible = 1;

        for (int i = 0; i < max_params; i++) {
                input[i][0] = 0;
        }

        /* Draw initial cursor */
        draw_cursor(shell_status.cursor.x, shell_status.cursor.y, 1);

        for_ever {
                char c = getchar();

                if (c == 0) {
                        uint64_t now = 0;
                        syscall_get_ticks(&now);
                        if (now - last_blink > CURSOR_BLINK_TICKS) {
                                if (cursor_visible) {
                                        erase_cursor(shell_status.cursor.x,
                                                     shell_status.cursor.y);
                                        cursor_visible = 0;
                                } else {
                                        draw_cursor(shell_status.cursor.x,
                                                    shell_status.cursor.y, 1);
                                        cursor_visible = 1;
                                }
                                last_blink = now;
                        }
                        continue;
                }

                if (c == ZOOM_IN_CHAR &&
                    shell_status.magnification < MAX_FONT_SCALE) {
                        shell_status.magnification++;
                        syscall_set_font_size(shell_status.magnification);
                        syscall_redraw_screen();
                        sync_cursor_pos();
                        continue;
                }

                if (c == ZOOM_OUT_CHAR &&
                    shell_status.magnification > MIN_FONT_SCALE) {
                        shell_status.magnification--;
                        syscall_set_font_size(shell_status.magnification);
                        syscall_redraw_screen();
                        sync_cursor_pos();
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

                if (c == '\b' && buf_idx > 0) {
                        if (cursor_visible) {
                                erase_cursor(shell_status.cursor.x,
                                             shell_status.cursor.y);
                        }
                        buf_idx--;
                        putchar('\b');
                        sync_cursor_pos();
                        draw_cursor(shell_status.cursor.x,
                                    shell_status.cursor.y, 1);
                        cursor_visible = 1;
                        syscall_get_ticks(&last_blink);
                        continue;
                }

                if (c == ' ' && buf_idx > 0) {
                        if (cursor_visible) {
                                erase_cursor(shell_status.cursor.x,
                                             shell_status.cursor.y);
                        }
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
                        syscall_get_ticks(&last_blink);
                        continue;
                }

                if (buf_idx < MAX_CHARS - 1) {
                        buffer[buf_idx++] = c;
                        putchar(c);
                        sync_cursor_pos();
                        draw_cursor(shell_status.cursor.x,
                                    shell_status.cursor.y, 1);
                        cursor_visible = 1;
                        syscall_get_ticks(&last_blink);
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
        printf(logo_shell);
        printf(helper_msg);
        sync_cursor_pos();

        for_ever {
                uint64_t current_ticks = 0;
                syscall_get_ticks(&current_ticks);
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
        return OK;
}
