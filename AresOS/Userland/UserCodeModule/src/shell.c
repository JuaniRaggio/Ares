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
#define CHECK_MAN "Type \"man %s\" to see how the command works\n"
#define PROMPT_LENGTH 3
#define SHELL_PIPE_NAME "sh-pipe"

#include <process_api.h>
#include <process_types.h>

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

/* getchar() blocks in the kernel until input arrives, so the shell does not
 * poll. The cursor stays solid while waiting. */
int shell_read_line(char input[][MAX_CHARS], int max_params) {
        char buffer[MAX_CHARS];
        int buf_idx       = 0;
        int param_count   = 0;
        int current_param = 0;

        for (int i = 0; i < max_params; i++) {
                input[i][0] = 0;
        }

        /* Draw initial cursor */
        draw_cursor(shell_status.cursor.x, shell_status.cursor.y, 1);

        for_ever {
                int c = getchar();

                if (c == EOF) {
                        /* Ctrl+D at the prompt: nothing to send EOF to */
                        printf("^D\n");
                        sync_cursor_pos();
                        return 0;
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
                        erase_cursor(shell_status.cursor.x,
                                     shell_status.cursor.y);
                        buf_idx--;
                        putchar('\b');
                        sync_cursor_pos();
                        draw_cursor(shell_status.cursor.x,
                                    shell_status.cursor.y, 1);
                        continue;
                }

                if (c == ' ' && buf_idx > 0) {
                        erase_cursor(shell_status.cursor.x,
                                     shell_status.cursor.y);
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
                                while ((c = getchar()) != '\n' && c != EOF)
                                        ;
                                putchar('\n');
                                sync_cursor_pos();
                                return param_count;
                        }
                        putchar(' ');
                        sync_cursor_pos();
                        draw_cursor(shell_status.cursor.x,
                                    shell_status.cursor.y, 1);
                        continue;
                }

                if (c >= ' ' && buf_idx < MAX_CHARS - 1) {
                        buffer[buf_idx++] = (char)c;
                        putchar(c);
                        sync_cursor_pos();
                        draw_cursor(shell_status.cursor.x,
                                    shell_status.cursor.y, 1);
                }
        }
}

/* Runs a shell built-in using the typed lambda dispatch. Built-ins execute
 * inside the shell process; everything else runs as a separate process. */
static void run_builtin(int idx, uint32_t params) {
        if (commands[idx]->lambda.ftype != (function_type)(params - 1)) {
                printf(wrong_params);
                printf(CHECK_MAN, shell_status.prompts.user_input[0]);
                return;
        }
        add_to_history(commands[idx], params);

        composed_command_t current_prompt =
            shell_status.prompts.prompt_history[lastest_prompt_idx()];
        executable_t execute = current_prompt.cmd->lambda.execute;
        switch (params - 1) {
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

/* Spawns a registered application as a process. Foreground commands are
 * waited for; background commands return to the prompt immediately. */
static void run_external(int tokens, int background) {
        char *argv[SHELL_MAX_TOKENS];
        uint64_t argc = tokens - 1;
        for (uint64_t i = 0; i < argc; i++) {
                argv[i] = shell_status.prompts.user_input[i + 1];
        }

        int64_t pid = my_spawn(shell_status.prompts.user_input[0], argc, argv,
                               !background, NO_PIPE, NO_PIPE);
        if (pid < 0) {
                printf("Could not create process\n");
                return;
        }

        if (background) {
                printf("[pid %d] running in background\n", (int)pid);
        } else {
                my_wait(pid);
        }
}

/* Connects two registered applications with a pipe: left | right.
 * The reader is created first so the writer always finds it attached. */
static void run_piped(int pipe_pos, int tokens) {
        char *left  = shell_status.prompts.user_input[0];
        char *right = shell_status.prompts.user_input[pipe_pos + 1];

        if (pipe_pos + 1 >= tokens) {
                printf("Syntax: p1 | p2\n");
                return;
        }
        if (!process_is_registered(left) || !process_is_registered(right)) {
                printf(invalid_command);
                return;
        }

        int pipe_id = my_pipe_open(SHELL_PIPE_NAME);
        if (pipe_id < 0) {
                printf("Could not open pipe\n");
                return;
        }

        char *largv[SHELL_MAX_TOKENS];
        uint64_t largc = pipe_pos - 1;
        for (uint64_t i = 0; i < largc; i++) {
                largv[i] = shell_status.prompts.user_input[i + 1];
        }

        char *rargv[SHELL_MAX_TOKENS];
        uint64_t rargc = tokens - pipe_pos - 2;
        for (uint64_t i = 0; i < rargc; i++) {
                rargv[i] = shell_status.prompts.user_input[pipe_pos + 2 + i];
        }

        int64_t right_pid = my_spawn(right, rargc, rargv, 1, pipe_id, NO_PIPE);
        if (right_pid < 0) {
                printf("Could not create process\n");
                my_pipe_close(pipe_id);
                return;
        }

        int64_t left_pid = my_spawn(left, largc, largv, 1, NO_PIPE, pipe_id);
        if (left_pid < 0) {
                printf("Could not create process\n");
                my_kill(right_pid);
                my_pipe_close(pipe_id);
                return;
        }

        my_wait(left_pid);
        my_wait(right_pid);
        my_pipe_close(pipe_id);
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
                printf(input_prompt);
                sync_cursor_pos();
                int r_tokens = shell_read_line(shell_status.prompts.user_input,
                                               SHELL_MAX_TOKENS);

                if (r_tokens == 0 ||
                    shell_status.prompts.user_input[0][0] == 0)
                        continue;

                /* A trailing "&" sends the command to the background */
                int background = FALSE;
                if (r_tokens > 1 &&
                    strcmp(shell_status.prompts.user_input[r_tokens - 1],
                           "&") == 0) {
                        background = TRUE;
                        shell_status.prompts.user_input[r_tokens - 1][0] = 0;
                        r_tokens--;
                }

                /* A "|" between two commands connects them with a pipe */
                int pipe_pos = -1;
                for (int i = 1; i < r_tokens - 1 && pipe_pos < 0; i++) {
                        if (strcmp(shell_status.prompts.user_input[i], "|") ==
                            0)
                                pipe_pos = i;
                }

                if (pipe_pos > 0) {
                        run_piped(pipe_pos, r_tokens);
                        continue;
                }

                int builtin =
                    get_command_index(shell_status.prompts.user_input[0]);
                if (builtin != INVALID_COMMAND_NAME) {
                        if (background) {
                                printf("Built-in commands cannot run in "
                                       "background\n");
                                continue;
                        }
                        run_builtin(builtin, r_tokens);
                        continue;
                }

                if (process_is_registered(
                        shell_status.prompts.user_input[0])) {
                        run_external(r_tokens, background);
                        continue;
                }

                printf(invalid_command);
        }
        return OK;
}