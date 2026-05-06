#pragma once

typedef enum {
        OK = 0,
        ABORT,
        INVALID_INPUT,
        VALID_INPUT,
        EMPTY,
} status_code;

#define SYS_OK     0
#define SYS_ERR   (-1)
#define SYS_BAD    1
