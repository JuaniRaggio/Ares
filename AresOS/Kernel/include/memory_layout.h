#pragma once

/**
 * @file memory_layout.h
 * @brief Physical memory layout of the system (single place to change it).
 *
 * Addresses are fixed by the bootloader / module loader: the user code and
 * data modules are loaded at known physical addresses, and the heap uses the
 * gaps around them. Centralized here so the layout can be adjusted in one spot.
 */

#include <stdint.h>

#define PAGE_SIZE 0x1000

/* Where the module loader places the user code and data modules. */
#define USER_CODE_MODULE_ADDR 0x400000
#define USER_DATA_MODULE_ADDR 0x500000

/* Second heap region: a fixed 32 MB block above the user modules. */
#define HEAP_REGION2_START 0x600000
#define HEAP_REGION2_SIZE  0x2000000 /* 32 MB */

/* The first heap region starts this many pages past the end of the kernel. */
#define HEAP_REGION1_PAGE_GAP 8
