#include <commands.h>
#include <configuration.h>
#include <tron.h>

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
        char user_input[3][256];
        void *prompt_history;
} prompt_data;

typedef struct {
        char buffer[1024 * 768];
        uint8_t magnification;
        float font_size;
        uint32_t font_color;
        uint32_t background_color;
        prompt_data prompts;
        shell_cursor cursor;
} shell_attributes;

extern shell_attributes shell_status;

void history_cmd(char **history) {
        int count = 0;
        printf("Command history:\n");
        for (int i = 0; history[i][i] != 0 && i < HISTORY_SIZE; i++) {
                if (history[i][0] != 0) {
                        printf("%2d: %s\n", ++count, history[i]);
                }
        }
        if (count == 0) {
                printf("No commands in history\n");
        }
}

void print_info_reg(void) {
        regs_snapshot_t regs;
        syscall_get_register_snapshot(&regs);
        printf("===== Register snapshot: =====\n");
        printf("      RIP: 0x%x\n", regs.rip);
        printf("      RSP: 0x%x\n", regs.rsp);
        printf("      RAX: 0x%x\n", regs.rax);
        printf("      RBX: 0x%x\n", regs.rbx);
        printf("      RCX: 0x%x\n", regs.rcx);
        printf("      RDX: 0x%x\n", regs.rdx);
        printf("      RBP: 0x%x\n", regs.rbp);
        printf("      RDI: 0x%x\n", regs.rdi);
        printf("      RSI: 0x%x\n", regs.rsi);
        printf("      R8:  0x%x\n", regs.r8);
        printf("      R9:  0x%x\n", regs.r9);
        printf("      R10: 0x%x\n", regs.r10);
        printf("      R11: 0x%x\n", regs.r11);
        printf("      R12: 0x%x\n", regs.r12);
        printf("      R13: 0x%x\n", regs.r13);
        printf("      R14: 0x%x\n", regs.r14);
        printf("      R15: 0x%x\n", regs.r15);
}

int get_command_index(char *command) {
        for (int idx = 0; idx < QTY_COMMANDS; idx++) {
                if (commands[idx]->name &&
                    !strcmp(commands[idx]->name, command))
                        return idx;
        }
        return INVALID_COMMAND_NAME;
}

void help(void) {
        printf("Available commands:\n");
        for (int i = 0; i < QTY_COMMANDS && commands[i]->name != 0; i++) {
                printf("  %s: %s\n", commands[i]->name,
                       commands[i]->description);
        }
}

int div_cmd(char *num_str, char *div_str) {
        int num = 0, div = 0;
        for (int i = 0; num_str[i] >= '0' && num_str[i] <= '9'; i++)
                num = num * 10 + (num_str[i] - '0');
        for (int i = 0; div_str[i] >= '0' && div_str[i] <= '9'; i++)
                div = div * 10 + (div_str[i] - '0');
        if (div == 0) {
                printf("Error: division by zero\n");
                return 0;
        }
        printf("%d / %d = %d\n", num, div, num / div);
        return 1;
}

void show_time(void) {
        uint64_t seconds = syscall_get_seconds();
        uint32_t h       = seconds / 3600;
        uint32_t m       = (seconds % 3600) / 60;
        uint32_t s       = seconds % 60;
        printf("Elapsed time: %d:%d:%d\n", h, m, s);
}

void clear_cmd(void) {
        syscall_clear();
}

void print_mem(char *pos_str) {
        uint64_t addr = 0;

        if (pos_str[0] == '0' && (pos_str[1] == 'x' || pos_str[1] == 'X'))
                pos_str += 2;

        for (int i = 0; pos_str[i]; i++) {
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
        for (int i = 0; i < 32; i++) {
                printf("%2x ", buffer[i]);
                if ((i + 1) % 8 == 0)
                        printf("\n");
        }
}

void man(char *command) {
        int idx = get_command_index(command);
        if (idx != -1) {
                printf("Command: %s\n", commands[idx]->name);
                printf("Description: %s\n", commands[idx]->description);
                printf("Parameters: %d\n", commands[idx]->lambda.ftype);
        } else {
                printf(invalid_command);
        }
}

int cursor_cmd(char *type) {
        if (!strcmp(type, "block")) {
                shell_status.cursor.shape = block;
                printf("Cursor shape set to: block\n");
        } else if (!strcmp(type, "hollow")) {
                shell_status.cursor.shape = hollow;
                printf("Cursor shape set to: hollow\n");
        } else if (!strcmp(type, "line")) {
                shell_status.cursor.shape = line;
                printf("Cursor shape set to: line\n");
        } else if (!strcmp(type, "underline")) {
                shell_status.cursor.shape = underline;
                printf("Cursor shape set to: underline\n");
        } else {
                printf("Invalid cursor type or not supported yet. Current "
                       "options: block, hollow, line, underline\nTo add a new "
                       "cursor type, contact support@ares.com");
                return 0;
        }
        return 1;
}

void tron_cmd(void) {
        tron_game();
}
