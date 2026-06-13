# AresOS Troubleshooting Log

Bugs found and fixed while bringing the kernel up to the TP2 requirements
(processes, scheduling, semaphores, pipes, memory managers). Each entry lists
the symptom, the root cause, and the fix. Ordered roughly by how hard they
were to diagnose.

---

## 1. `sysretq` silently assembles to nothing → no real ring 3

### Symptom
Everything "worked" but isolation was an illusion: after the first syscall the
CPU never returned to user mode. The bug stayed hidden until syscall return
values started being used (waitpid/foreground), at which point running an app
left the shell unable to recover.

### Root Cause
The NASM 2.16.03 shipped in the mandatory image `agodio/itba-so-multiarch:3.1`
does not accept the `sysretq` mnemonic. It parses it as a stray label and
emits **zero bytes** (this was the recurring `label alone on a line without a
colon` warning). With no `sysret`, the syscall return path fell through into
`init_syscalls`, which clobbered RAX (every syscall's return value) and ran
`ret` with the user stack, never lowering to ring 3.

It used to work because the old README pulled `agodio/itba-so:2.0`, whose NASM
*does* assemble `sysretq`.

### Fix
`Kernel/asm/syscalls.asm`: replace `sysretq` with `o64 sysret`, which assembles
to the correct opcode `48 0F 07` on this NASM. Verified byte-for-byte and by
sampling CPU registers in QEMU (CS=0x23/SS=0x1B, DPL=3 in userland).

---

## 2. SYSRET builds user selectors the GDT did not provide → triple fault on preemption

### Symptom
`test_mm` (and anything spending real time in ring 3) rebooted the machine
within ~100 ms. `mem`/`ps` survived because they spend almost all their time
inside syscalls. After a reboot the heap counters reset (Allocations back to 5),
which is what gave it away: a userland restart does not reinitialize the heap, a
full machine reset does.

### Root Cause
`init_syscalls` loaded `STAR[63:48] = 0x18`, but `sysret` derives the user code
selector as `STAR[63:48] + 16`. That produced `CS = 0x28`, which in the GDT was
the **TSS descriptor**, not user code. `sysret` does not validate the selector,
so syscalls returned "fine" with a bogus CS=0x2B. The validation happens in
`iretq`: when the timer preempted a process running in ring 3 and the scheduler
resumed it, the CPU loaded CS=0x2B, found a TSS where code was expected, and
raised `#GP(0x28)` — vector 13, which has no IDT handler → double fault →
triple fault → reset.

### Fix
Adopt the standard layout (same as Linux): user **data** before user **code** so
the `+8`/`+16` arithmetic lands correctly.
- `Kernel/asm/setup_user_segments.asm`: entry 3 (0x18) = user data, entry 4
  (0x20) = user code.
- `Kernel/asm/init_syscalls.asm`: `STAR[63:48] = 0x10` → sysret yields SS=0x1B,
  CS=0x23.
- `Kernel/core/process.c`: `USER_CS 0x23`, `USER_SS 0x1B`.
- `Kernel/asm/jump_to_userland.asm`: push the same selectors.

Verified: with `-d int`, zero exceptions during a full `test_mm` run (before the
fix the log showed `v=0d e=0028` followed by `v=08`).

---

## 3. `process_wait` returned before the child actually died

### Symptom
A foreground command would print its output but the shell prompt would not come
back, or came back with a garbage exit code.

### Root Cause
`process_wait` blocked, called `scheduler_yield()`, then checked the child's
state **once** and returned. But `scheduler_yield()` only drops the quantum —
the real context switch happens on the next timer tick — so the check ran before
the child had a chance to finish. It also set `state = BLOCKED` before setting
`waiting_for`, so a tick landing between those two writes would block the waiter
with a stale `waiting_for`, losing the wakeup.

### Fix
`Kernel/core/process.c`: set `waiting_for` *before* `BLOCKED`, and loop on
`_hlt()` until the child really reaches ZOMBIE/DEAD, instead of checking once.

---

## 4. `sem_wait` returned to userland while still BLOCKED

### Symptom
Semaphore-protected sections were not actually mutually exclusive in edge cases;
a process could double-enqueue itself on a semaphore, permanently unbalancing
its counter.

### Root Cause
`sem_wait` marked the process BLOCKED, called `scheduler_yield()`, and
**returned**. Since yield only drops the quantum, the process kept executing
userland instructions until the next tick despite being BLOCKED — and could
re-enter `sem_wait`, enqueueing its pid twice for one wait.

### Fix
`Kernel/core/semaphores.c`: after blocking, sleep on `_hlt()` until the BLOCKED
state is cleared by `sem_post` (or a kill), mirroring `process_wait`. Also clean
up the just-enqueued node if `block_by_semaphore` fails.
Verified: `test_sync 5 1` → `Final value: 0`; `mvar 2 2` alternates `ABAB...`.

---

## 5. Killing a process blocked on a semaphore left a stale queue node

### Symptom
After Ctrl+C on a process waiting on a semaphore, a busy semaphore could slowly
lose `value` accounting and eventually deadlock its remaining users.

### Root Cause
`process_kill` / `process_exit` cleaned up pipes but not semaphore wait queues.
A killed waiter's node stayed in the queue; a later `sem_post` dequeued the dead
pid, failed to wake it (it is ZOMBIE), and swallowed the post — starving the
next real waiter.

### Fix
New `sem_remove_from_queues(pid)` in `Kernel/core/semaphores.c`, called from both
`process_kill` and `process_exit`. It removes the pid from every queue and does
`value++` to undo the dead process's decrement, keeping counter and queue
consistent. It uses `irq_save`/`irq_restore` (not bare `_cli`/`_sti`) because it
also runs from the keyboard IRQ via `process_kill_foreground` (Ctrl+C), where a
bare `_sti` would wrongly re-enable interrupts inside the handler.

---

## 6. Memory manager free list corrupted by preemption

### Symptom
Latent (no consistent crash), but reachable under process churn (test_proc):
two processes touching the heap around a context switch could corrupt the free
list, leading to cycles or double-allocation.

### Root Cause
The free list is global and shared by every process, with no protection. A timer
tick can preempt an allocation mid-walk and switch to another process that also
mutates the list. The original assumption ("syscalls are one-at-a-time") ignored
that the context switch itself can interleave two half-finished heap operations.

### Fix
`Kernel/lib/multi_region_heap.c` and `Kernel/lib/buddy.c`: run `mem_alloc`,
`mem_free`, and `mem_get_stats` with interrupts disabled, using new
`irq_save`/`irq_restore` primitives (`Kernel/arch/x86_64/interrupts/interrupts.asm`).
These save/restore the IF flag instead of unconditionally re-enabling it, so they
nest correctly with the semaphore paths that already hold `_cli` when they reach
the slab/heap. (A bare `_cli`/`_sti` inside `mem_alloc` would re-enable
interrupts in the middle of a semaphore critical section.)

---

## 7. FPU/SSE state not preserved across context switches (known limitation)

### Symptom
None observable. `test_sync` and `mvar` (which call `GetUniform` concurrently)
produce correct results without crashing.

### Root Cause
`pushState`/`popState` save only the 15 general-purpose registers, never the
XMM/x87 state. `GetUniform` (in the cátedra-provided `test_util.c`) uses `double`
arithmetic, which compiles to SSE. Two processes evaluating it concurrently can
clobber each other's XMM registers.

### Status
Left as a documented limitation rather than fixed, because:
- The impact is benign: the corrupted value only changes a *random* decision
  (whether `slowInc` yields, how long `mvar` busy-waits). It never affects
  correctness — semaphores still serialize, `test_sync` still returns 0 — and
  never crashes (verified empirically with the original float `GetUniform`).
- The proper fix (`fxsave`/`fxrstor` per process, plus 16-byte-aligned save
  areas and FPU init) adds real complexity to the context-switch hot path.
- The test is cátedra-provided and must not be modified.

If full FP support across processes is ever needed, add `fxsave [area]` /
`fxrstor [area]` around the switch in `_irq00Handler` with a per-PCB aligned save
area. (`CR4.OSFXSR` and `finit` are already set by Pure64.)

---

## 8. Shell cursor never visible — dead 768 KB field shifted live fields out of .data

### Symptom
The shell cursor was never drawn, at the prompt or while typing. Long-standing,
predating the syscall rework.

### Root Cause
`shell_attributes` had `char buffer[SCREEN_SIZE]` (768 KB, `1024*768`) as its
**first** field, unused by anything. `shell_status` is a statically-initialized
global, so it lives in `.data`; the 768 KB dead field pushed `magnification`,
`font_width` and `font_height` ~768 KB into the segment, past what the flat
userland binary actually loaded with initial values. At runtime those fields
read as 0, so `draw_cursor` computed `px = x * font_width * scale = 0` and
`py = 0` — the cursor was always drawn at (0,0), hidden behind the banner.

Diagnosed by: a fixed test rect drew fine (syscall works); forcing the cursor
to a fixed position drew fine (draw_cursor runs); encoding `cursor.x`/`cursor.y`
as rect positions showed them correct (3, 27); but drawing at the real
`(px,py)` landed at (0,0) — proving `font_width`/`magnification` were 0.

### Fix
Remove the unused `buffer[SCREEN_SIZE]` field from `shell_attributes`
(`include/shell.h`). With it gone, `.data` loads the initialized fields
correctly and the cursor renders at the right spot. Side effect: the userland
binary shrank from ~843 KB to ~52 KB.

---

## Notes for the README "limitations" section
- Killing a process mid-execution leaks its outstanding heap blocks: the kernel
  does not track block ownership per process.
- A single pipe per command line (`p1 | p2`), space-separated `|` and `&`, and
  no background pipes (`p1 | p2 &`).
- The shell reports `STACK BASE 0x0` for pid 0 because it runs on the static
  kernel stack rather than a heap-allocated one.
