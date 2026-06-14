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

/* find_pipe_position() returns a token index; this means the line has no "|". */
#define NO_PIPE_TOKEN (-1)

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
static void save_to_history(int tokens);

shell_attributes shell_status = {
    .magnification    = MIN_FONT_SCALE,
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

/* Rebuilds the typed line from its tokens and stores it in the circular
 * history. Registers every command (built-ins and applications). */
static void save_to_history(int tokens) {
        char *slot =
            shell_status.prompts
                .history[shell_status.prompts.history_count % HISTORY_SIZE];
        int pos = 0;
        for (int t = 0; t < tokens && pos < MAX_CHARS - 1; t++) {
                if (t > 0 && pos < MAX_CHARS - 1)
                        slot[pos++] = ' ';
                for (int j = 0; shell_status.prompts.user_input[t][j] &&
                                pos < MAX_CHARS - 1;
                     j++) {
                        slot[pos++] = shell_status.prompts.user_input[t][j];
                }
        }
        slot[pos] = '\0';
        shell_status.prompts.history_count++;
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
                          shell_status.font_height * scale,
                          shell_status.background_color);
}

/* Copies the finished token from buffer into input[param] */
static void store_token(char input[][MAX_CHARS], int param,
                        const char *buffer, int len) {
        int i;
        for (i = 0; i < len && i < MAX_CHARS - 1; i++) {
                input[param][i] = buffer[i];
        }
        input[param][i] = 0;
}

/* Redraws the cursor at the current screen position */
static void refresh_cursor(void) {
        sync_cursor_pos();
        draw_cursor(shell_status.cursor.x, shell_status.cursor.y, 1);
}

/* Handles Ctrl+- / Ctrl+= font scaling. Returns TRUE if c was a zoom key */
static int handle_zoom_keys(int c) {
        if (c == ZOOM_IN_CHAR && shell_status.magnification < MAX_FONT_SCALE) {
                shell_status.magnification++;
        } else if (c == ZOOM_OUT_CHAR &&
                   shell_status.magnification > MIN_FONT_SCALE) {
                shell_status.magnification--;
        } else {
                return FALSE;
        }
        syscall_set_font_size(shell_status.magnification);
        syscall_redraw_screen();
        sync_cursor_pos();
        return TRUE;
}

/* Swallows the rest of the line once the token limit was reached */
static void discard_until_newline(void) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
                ;
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

                if (handle_zoom_keys(c))
                        continue;

                if (c == '\n') {
                        if (buf_idx > 0) {
                                store_token(input, current_param, buffer,
                                            buf_idx);
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
                        refresh_cursor();
                        continue;
                }

                if (c == ' ' && buf_idx > 0) {
                        erase_cursor(shell_status.cursor.x,
                                     shell_status.cursor.y);
                        store_token(input, current_param, buffer, buf_idx);
                        current_param++;
                        param_count++;
                        buf_idx = 0;

                        if (current_param >= max_params) {
                                discard_until_newline();
                                putchar('\n');
                                sync_cursor_pos();
                                return param_count;
                        }
                        putchar(' ');
                        refresh_cursor();
                        continue;
                }

                if (c >= ' ' && buf_idx < MAX_CHARS - 1) {
                        /* Clear the cursor mark before the glyph overwrites
                         * the cell, otherwise the underline is left behind. */
                        erase_cursor(shell_status.cursor.x,
                                     shell_status.cursor.y);
                        buffer[buf_idx++] = (char)c;
                        putchar(c);
                        refresh_cursor();
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

        /* user_input[0] is the command; [1] and [2] are its arguments. */
        executable_t execute = commands[idx]->lambda.execute;
        switch (params - 1) {
        case supplier_t:
                execute.supplier();
                break;
        case function_t:
                execute.function(shell_status.prompts.user_input[1]);
                break;
        case bi_function_t:
                execute.bi_function(shell_status.prompts.user_input[1],
                                    shell_status.prompts.user_input[2]);
                break;
        }
}

/* Fills argv with the tokens in [from, to) and returns argc */
static uint64_t collect_args(int from, int to, char *argv[]) {
        uint64_t argc = 0;
        for (int i = from; i < to; i++) {
                argv[argc++] = shell_status.prompts.user_input[i];
        }
        return argc;
}

/* Spawns a registered application as a process. Foreground commands are
 * waited for; background commands return to the prompt immediately. */
static void run_external(int tokens, int background) {
        char *argv[SHELL_MAX_TOKENS];
        uint64_t argc = collect_args(1, tokens, argv);

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
        uint64_t largc = collect_args(1, pipe_pos, largv);

        char *rargv[SHELL_MAX_TOKENS];
        uint64_t rargc = collect_args(pipe_pos + 2, tokens, rargv);

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

/* Detects and removes a trailing "&". Returns TRUE if the command should
 * run in background */
static int consume_background_token(int *tokens) {
        if (*tokens < 2)
                return FALSE;
        char *last = shell_status.prompts.user_input[*tokens - 1];
        if (strcmp(last, "&") != 0)
                return FALSE;
        last[0] = 0;
        (*tokens)--;
        return TRUE;
}

/* Returns the position of the "|" token, or NO_PIPE_TOKEN if there is none. */
static int find_pipe_position(int tokens) {
        for (int i = 1; i < tokens - 1; i++) {
                if (strcmp(shell_status.prompts.user_input[i], "|") == 0)
                        return i;
        }
        return NO_PIPE_TOKEN;
}

/* Resolves a single (non piped) command: built-in, application or error */
static void run_single(int tokens, int background) {
        int builtin = get_command_index(shell_status.prompts.user_input[0]);
        if (builtin != INVALID_COMMAND_NAME) {
                if (background) {
                        printf("Built-in commands cannot run in background\n");
                        return;
                }
                run_builtin(builtin, tokens);
                return;
        }

        if (process_is_registered(shell_status.prompts.user_input[0])) {
                run_external(tokens, background);
                return;
        }

        printf(invalid_command);
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

                save_to_history(r_tokens);

                int background = consume_background_token(&r_tokens);
                int pipe_pos   = find_pipe_position(r_tokens);

                if (pipe_pos > 0) {
                        run_piped(pipe_pos, r_tokens);
                } else {
                        run_single(r_tokens, background);
                }
        }
        return OK;
}