#pragma once

/**
 * @file scheduler.h
 * @brief Round-robin scheduler with priority-based quantum.
 *
 * Called from the timer ISR to perform context switches. Higher priority
 * processes get more timer ticks before being preempted.
 */

#include <stdint.h>

/** @brief Initialize the scheduler for the first process (pid 0). */
void scheduler_init(void);

/**
 * @brief Select the next process to run.
 *
 * Called from the timer ISR with the current kernel RSP (after pushState).
 * Saves the RSP into the current PCB, picks the next READY process, updates
 * TSS RSP0 and the per-process kernel stack pointer, and returns the new
 * process's saved RSP.
 *
 * @param current_rsp The current kernel stack pointer (points to saved context).
 * @return The RSP of the next process to run.
 */
uint64_t schedule(uint64_t current_rsp);
