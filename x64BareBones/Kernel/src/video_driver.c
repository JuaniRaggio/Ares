#include "video_driver.h"

void printLn(const char *str, const uint8_t color) {
        for (int i = 0; str[i] != 0; ++i) {
                ncPrintChar(str[i], color);
        }
        ncNewline();
}
