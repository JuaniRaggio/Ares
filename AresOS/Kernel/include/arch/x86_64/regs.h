#pragma once

#include <stdint.h>

/**
 * CPU register snapshot structure
 * Used to capture and store the state of all general-purpose registers
 *
 * IMPORTANT: Order matches stack layout after pushState + CPU interrupt:
 * r15, r14, r13, r12, r11, r10, r9, r8, rsi, rdi, rbp, rdx, rcx, rbx, rax,
 * RIP, CS, RFLAGS, RSP, SS
 */
typedef struct {
        uint64_t r15;
        uint64_t r14;
        uint64_t r13;
        uint64_t r12;
        uint64_t r11;
        uint64_t r10;
        uint64_t r9;
        uint64_t r8;
        uint64_t rsi;
        uint64_t rdi;
        uint64_t rbp;
        uint64_t rdx;
        uint64_t rcx;
        uint64_t rbx;
        uint64_t rax;
        uint64_t rip;
        uint64_t cs;
        uint64_t rflags;
        uint64_t rsp;
        uint64_t ss;
} regs_snapshot_t;
