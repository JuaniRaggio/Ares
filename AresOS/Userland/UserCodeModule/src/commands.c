#include <benchmarks.h>
#include <commands.h>
#include <shell.h>
#include <stdint.h>
#include <stdio.h>

extern shell_attributes shell_status;

uint8_t history_cmd(void) {
        printf("Command not supported yet! Sorry :(\n");
        return INVALID_COMMAND_NAME;
        // if (shell_status.prompts.lastest_prompt_idx == 0) {
        //         printf("Empty history!\n");
        //         return EMPTY;
        // }
        // printf("Command history:\n");
        // for (uint8_t i = 0; i < shell_status.prompts.lastest_prompt_idx; i++)
        // {
        //         printf("%s", shell_status.prompts.prompt_history[i]);
        //         for (uint8_t j = 0; j < max_parameters; ++j) {
        //                 if (shell_status.prompts.prompt_history[i].args[j][0]
        //                 !=
        //                     0) {
        //                         printf(" %s",
        //                                shell_status.prompts.prompt_history[i]
        //                                    .args[j]);
        //                 }
        //         }
        //         putchar('\n');
        // }
        return OK;
}

uint8_t print_info_reg(void) {
        regs_snapshot_t regs;
        uint64_t ret = syscall_get_register_snapshot(&regs);
        printf("===== Register snapshot: =====\n");
        printf("      RIP:    0x%x\n", regs.rip);
        printf("      RSP:    0x%x\n", regs.rsp);
        printf("      RAX:    0x%x\n", regs.rax);
        printf("      RBX:    0x%x\n", regs.rbx);
        printf("      RCX:    0x%x\n", regs.rcx);
        printf("      RDX:    0x%x\n", regs.rdx);
        printf("      RBP:    0x%x\n", regs.rbp);
        printf("      RDI:    0x%x\n", regs.rdi);
        printf("      RSI:    0x%x\n", regs.rsi);
        printf("      R8:     0x%x\n", regs.r8);
        printf("      R9:     0x%x\n", regs.r9);
        printf("      R10:    0x%x\n", regs.r10);
        printf("      R11:    0x%x\n", regs.r11);
        printf("      R12:    0x%x\n", regs.r12);
        printf("      R13:    0x%x\n", regs.r13);
        printf("      R14:    0x%x\n", regs.r14);
        printf("      R15:    0x%x\n", regs.r15);
        printf("      CS:     0x%x\n", regs.cs);
        printf("      SS:     0x%x\n", regs.ss);
        printf("      RFLAGS: 0x%x\n", regs.rflags);
        return ret;
}

int get_command_index(char *command) {
        for (uint8_t idx = 0; idx < QTY_COMMANDS; idx++) {
                if (commands[idx]->name &&
                    !strcmp(commands[idx]->name, command))
                        return idx;
        }
        return INVALID_COMMAND_NAME;
}

uint8_t help(void) {
        printf("Available commands:\n");
        for (uint8_t i = 0; i < QTY_COMMANDS; i++) {
                printf("  %s: %s\n", commands[i]->name,
                       commands[i]->description);
        }
        return OK;
}

uint8_t div_cmd(char *num_str, char *div_str) {
        uint8_t num = 0, div = 0;
        for (uint8_t i = 0; num_str[i] >= '0' && num_str[i] <= '9'; i++)
                num = num * 10 + (num_str[i] - '0');
        for (uint8_t i = 0; div_str[i] >= '0' && div_str[i] <= '9'; i++)
                div = div * 10 + (div_str[i] - '0');
        printf("%d / %d = %d\n", num, div, num / div);
        return OK;
}

s_time get_elapsed_time(s_time current_time) {
        s_time start = get_shell_start_time();

        uint32_t current_total_seconds = current_time.hours * 3600 +
                                         current_time.minutes * 60 +
                                         current_time.seconds;
        uint32_t start_total_seconds =
            start.hours * 3600 + start.minutes * 60 + start.seconds;

        uint32_t elapsed_seconds;
        if (current_total_seconds >= start_total_seconds) {
                elapsed_seconds = current_total_seconds - start_total_seconds;
        } else {
                elapsed_seconds =
                    (86400 - start_total_seconds) + current_total_seconds;
        }

        s_time elapsed;
        elapsed.hours   = (elapsed_seconds / 3600) % 24;
        elapsed.minutes = (elapsed_seconds / 60) % 60;
        elapsed.seconds = elapsed_seconds % 60;

        return elapsed;
}

uint8_t show_time(void) {
        s_time time;
        syscall_get_time(&time);
        printf("Current time: %d:%d:%d\n", time.hours, time.minutes,
               time.seconds);

        s_time elapsed = get_elapsed_time(time);
        printf("Time elapsed: %d:%d:%d\n", elapsed.hours, elapsed.minutes,
               elapsed.seconds);
        return OK;
}

uint8_t clear_cmd(void) {
        syscall_set_bg_color(shell_status.background_color);
        syscall_clear();
        return OK;
}

uint8_t print_mem(char *pos_str) {
        uint64_t addr = 0;

        if (pos_str[0] == '0' && (pos_str[1] == 'x' || pos_str[1] == 'X'))
                pos_str += 2;

        for (uint8_t i = 0; pos_str[i]; i++) {
                char c = pos_str[i];
                addr <<= 4;
                if (c >= '0' && c <= '9')
                        addr += c - '0';
                else if (c >= 'a' && c <= 'f')
                        addr += c - 'a' + 10;
                else if (c >= 'A' && c <= 'F')
                        addr += c - 'A' + 10;
        }

        uint8_t buffer[32];
        syscall_get_memory(addr, buffer, 32);

        printf("Memory at 0x%x:\n", addr);
        for (uint8_t i = 0; i < 32; i++) {
                printf("%x ", buffer[i]);
                if ((i + 1) % 8 == 0)
                        printf("\n");
        }
        return OK;
}

uint8_t man(char *command) {
        int idx = get_command_index(command);
        if (idx == -1) {
                printf(invalid_command);
                return INVALID_INPUT;
        }
        printf("Command: %s\n", commands[idx]->name);
        printf("Description: %s\n", commands[idx]->description);
        printf("Parameters: %d\n", commands[idx]->lambda.ftype);
        return OK;
}

uint8_t cursor_cmd(char *type) {
        printf("Cursor not supported yet! Sorry :(\n");
        // if (!strcmp(type, "block")) {
        //         shell_status.cursor.shape = block;
        //         printf("Cursor shape set to: block\n");
        // } else if (!strcmp(type, "hollow")) {
        //         shell_status.cursor.shape = hollow;
        //         printf("Cursor shape set to: hollow\n");
        // } else if (!strcmp(type, "line")) {
        //         shell_status.cursor.shape = line;
        //         printf("Cursor shape set to: line\n");
        // } else if (!strcmp(type, "underline")) {
        //         shell_status.cursor.shape = underline;
        //         printf("Cursor shape set to: underline\n");
        // } else {
        //         printf("Invalid cursor type or not supported yet. Current "
        //                "options: block, hollow, line, underline\nTo add a new
        //                " "cursor type, contact support@ares.com");
        //         return INVALID_INPUT;
        // }
        return INVALID_COMMAND_NAME;
}

static void wait(char *msg) {
        printf("%s", msg);
        while (getchar() == 0)
                ;
}

uint8_t benchmark_cmd(void) {
        static const char *const init_benchmark_msg =
            "============ Ares OS Benchmarks ============\n\n";
        static const char *const end_benchmark_msg =
            "\n===== Benchmark Completed Successfully =====\n";
        static const uint8_t desired_tests = 20;

        printf("%s", init_benchmark_msg);

        fps_data fps = fps_benchmark(desired_tests);
        show_fps_benchmark(fps);

        wait("Press Any key to continue with the next test\n");

        timer_data timer = timer_benchmark(1000);
        show_timer_benchmark(timer);

        wait("Press Any key to continue with the next test\n");

        keyboard_data keyboard = keyboard_benchmark(100);
        show_keyboard_benchmark(keyboard);

        clear_cmd();

        printf("Resume:\n");
        printf("============================================\n");
        show_fps_benchmark(fps);
        show_timer_benchmark(timer);
        show_keyboard_benchmark(keyboard);
        printf("%s\n", end_benchmark_msg);
        return OK;
}

uint8_t tron_cmd(void) {
        tron_game();
        return OK;
}

extern void opcode_asm(void);

uint8_t trigger_opcode_cmd(void) {
        opcode_asm();
        return OK;
}

static uint32_t parse_color(char *color_str) {
        /* Try to parse as color name */
        if (strcmp(color_str, "black") == 0)
                return BLACK;
        if (strcmp(color_str, "white") == 0)
                return WHITE;
        if (strcmp(color_str, "red") == 0)
                return RED;
        if (strcmp(color_str, "green") == 0)
                return GREEN;
        if (strcmp(color_str, "blue") == 0)
                return BLUE;
        if (strcmp(color_str, "yellow") == 0)
                return YELLOW;
        if (strcmp(color_str, "cyan") == 0)
                return CYAN;
        if (strcmp(color_str, "magenta") == 0)
                return MAGENTA;
        if (strcmp(color_str, "gray") == 0)
                return GRAY;
        if (strcmp(color_str, "lightgray") == 0)
                return LIGHT_GRAY;
        if (strcmp(color_str, "darkgray") == 0)
                return DARK_GRAY;

        /* Try to parse as hex value (0xRRGGBB) */
        if (color_str[0] == '0' &&
            (color_str[1] == 'x' || color_str[1] == 'X')) {
                return (uint32_t)strtoul(color_str, NULL, 16);
        }

        return 0xFFFFFFFF; /* Invalid color */
}

uint8_t textcolor_cmd(char *color) {
        uint32_t color_value = parse_color(color);

        if (color_value == 0xFFFFFFFF) {
                printf("Invalid color: %s\n", color);
                printf("Valid colors: black, white, red, green, blue, yellow, "
                       "cyan, magenta, gray, lightgray, darkgray\n");
                printf("Or use hex format: 0xRRGGBB\n");
                return INVALID_INPUT;
        }

        shell_status.font_color = color_value;
        syscall_set_text_color(color_value, STDOUT);
        printf("Text color changed\n");
        return OK;
}

uint8_t bgcolor_cmd(char *color) {
        uint32_t color_value = parse_color(color);

        if (color_value == 0xFFFFFFFF) {
                printf("Invalid color: %s\n", color);
                printf("Valid colors: black, white, red, green, blue, yellow, "
                       "cyan, magenta, gray, lightgray, darkgray\n");
                printf("Or use hex format: 0xRRGGBB\n");
                return INVALID_INPUT;
        }

        shell_status.background_color = color_value;
        syscall_set_bg_color(color_value);
        printf("Background color changed\n");
        return OK;
}
