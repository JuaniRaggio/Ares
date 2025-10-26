#include <shell.h>
#include <stdint.h>

const uint64_t screen_size = TEXT_WIDTH * TEXT_HEIGHT;

void welcome_shell() {
}

int shell(void) {
        uint8_t buffer[screen_size] = {0};
        uint8_t running_shell       = RUNNING;
        welcome_shell();

        while (running_shell) {
                //
        }

        return 0;
}
