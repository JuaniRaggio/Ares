# Bug Fix: Random Resets During Kernel Initialization

## Issue

System was randomly resetting during kernel initialization with non-deterministic behavior. Debug prints showed the system sometimes reached DEBUG 7, sometimes DEBUG 8, and sometimes DEBUG 13 before resetting.

## Root Cause

**Interrupts were being enabled BEFORE the heap, scheduler, and process system were fully initialized.**

When `enable_interrupts()` was called too early, a timer interrupt could fire before the scheduler was ready. This would attempt a context switch with uninitialized structures, causing a triple fault and system reset.

## Solution

**Move `enable_interrupts()` to the END of the initialization sequence.**

### Correct Initialization Order in `kernel.c::main()`:

```c
int main() {
    // 1. Basic hardware setup
    video_init();
    timer_init();
    load_idt();
    init_syscalls();
    setup_user_segments();
    setup_tss();

    // 2. Core subsystems (require hardware to be ready)
    init_heap();           // ← Heap FIRST
    process_init();        // ← Processes SECOND
    scheduler_init();      // ← Scheduler THIRD
    sem_system_init();     // ← Semaphores FOURTH

    // 3. Graphics setup
    clearScreen(BLACK);
    setFont(&font_ubuntu_mono);
    restore_cursor();

    // 4. ENABLE INTERRUPTS LAST ← CRITICAL!
    enable_interrupts();

    // 5. Jump to userland
    jump_to_userland(userCodeModuleAddress);

    for_ever _hlt();
    return 0;
}
```

## Key Principle

> **NEVER enable interrupts before all critical subsystems (heap, processes, scheduler) are fully initialized.**

The timer will immediately start generating interrupts, and if the scheduler isn't ready, the system will crash.

## Date Fixed
June 8, 2026

## Symptoms Before Fix
- Random system resets during boot
- Non-deterministic behavior
- Triple faults
- Sometimes crashed at different initialization points

## Symptoms After Fix
- Stable, deterministic boot sequence
- All debug messages print in order
- System successfully reaches userland
