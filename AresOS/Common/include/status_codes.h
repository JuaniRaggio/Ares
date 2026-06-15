#pragma once

/** Generic operation status (category 1). */
typedef enum {
        SYS_OK  = 0,
        SYS_ERR = -1,
        SYS_BAD = 1,
} sys_status_t;

/** Shell command / parser result. */
typedef enum {
        OK = 0,
        ABORT,
        INVALID_INPUT,
        VALID_INPUT,
        EMPTY,
} status_code;
