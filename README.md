# ARES: A Recursive Experimental System

Operating Systems TP2 (ITBA) — the kernel of a 64-bit monolithic operating
system built on top of the Computer Architecture TP. It implements physical
memory management (two interchangeable allocators), processes with
priority-based Round Robin scheduling (priority sets how often a process is
scheduled), semaphores, pipes, and a set of user applications that all run as
processes.

## Highlights

Notable design choices, mapped to each required area:

- **Memory management** — two interchangeable allocators (first-fit and buddy),
  chosen at build time behind a single interface. `mem` reports live statistics
  that match an analytic per-process model to the byte (see
  `doc/memory_analysis.pdf`).
- **Processes** — there are no built-ins: the shell spawns *every* command and
  test as a real process, so all of them can be backgrounded (`&`) or piped
  (`|`). When a process dies the kernel reaps any orphan zombie and re-parents
  its living children to the shell, so no kernel resources are leaked.
- **Scheduling / priority** — priority controls how *often* a process is selected
  (deficit round robin), not how long it runs. Its effect is therefore observable
  both for CPU-bound work (`test_prio`) and for cooperative, yield-bound work
  (`mvar` with `nice`). `yield()` forces an immediate context switch (software
  vector `0x81`) instead of waiting for the next timer tick.
- **Synchronization** — named semaphores shareable by unrelated processes (via an
  agreed string id) and reference-counted. The system has no deadlocks, race
  conditions, or busy waiting except where the assignment allows it — the sound
  driver included, which sleeps instead of spinning.
- **IPC** — pipes with full terminal/pipe transparency: a command does not know
  whether its stdin/stdout is the console or a pipe.
- **Exceptions** — a divide-by-zero or invalid opcode terminates *only* the
  faulting process (the kernel reports its name and pid), returns control to the
  scheduler, and the rest of the system keeps running; nothing is restarted.
- **Context switch** — saves and restores the FPU/SSE state per process
  (`fxsave`/`fxrstor`), not only the general-purpose registers.

## Building and running

Building must happen inside the image provided by the course:

```bash
# 1. Pull the image (once)
docker pull agodio/itba-so-multiarch:3.1

# 2. Create the container mounting the project (from Ares/AresOS)
cd AresOS
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -it \
    --name ARES agodio/itba-so-multiarch:3.1

# 3. Build
./compile_in_container.sh ARES          # default memory manager (first-fit)
./compile_in_container.sh ARES buddy    # buddy system
./compile_in_container.sh ARES clean    # clean

# 4. Run in QEMU (on the host)
./run.sh                                  # normal
./run.sh -d                               # debug mode (GDB on port 1234)
```

`make` rules (run inside the container):

- `make` / `make all` — build with the default memory manager.
- `make buddy` — build with the buddy system.
- `make clean` — remove build artifacts.

The memory manager is selected at **build time**: both implement the same
interface (`mem_alloc`/`mem_free`/`mem_get_stats`, see
`Kernel/include/lib/memory_manager.h`) and only one is linked. Building with
`-Wall` reports no warnings.

## IDE setup

`compile_commands.json` is generated on the **host** (not in Docker) so clangd
resolves local paths:

```bash
cd AresOS && make compile_commands
```

Re-run it when source files are added, moved, or removed. The shared clangd
configuration lives in `AresOS/.clangd`.

## Commands and tests

### Commands (everything runs as a process)

There is no built-in/process distinction: the user shell is the process `sh`
(PID 0, always running) and resolves every command **by name**, spawning it as a
separate process. That is why any command can run in the background (`&`) or be
connected through a pipe (`|`) — for example `help | wc`.

| Command | Parameters | Description |
|---|---|---|
| `help` | — | List every command and the course tests. |
| `man` | `<command>` | Show the help for a command. |
| `mem` | — | Memory manager state (total, used, free, blocks, allocs/frees). |
| `ps` | — | List processes: PID, name, priority, state, foreground, RSP, stack base. |
| `loop` | `[seconds]` | Print its PID every N seconds via active wait (default 2s). |
| `kill` | `<pid>` | Kill the process with the given PID. |
| `nice` | `<pid> <priority>` | Change a process priority (1 to 4). |
| `block` | `<pid>` | Toggle a process between blocked and ready. |
| `cat` | — | Print stdin as it is received. |
| `wc` | — | Count the number of lines in the input. |
| `filter` | — | Filter the vowels out of the input. |
| `mvar` | `<writers> <readers>` | Multiple readers/writers over a global variable (Haskell-style MVar). |
| `time` | — | System time and elapsed time. |
| `clear` | — | Clear the screen. |
| `history` | — | Show the command history. |
| `inforeg` | — | CPU registers captured with Ctrl+R. |
| `cursor` | `<shape>` | Change the cursor shape (block/hollow/line/underline). |
| `textcolor` | `<color>` | Change the text color. |
| `bgcolor` | `<color>` | Change the background color. |
| `div` | `<a> <b>` | Integer division (demonstrates the divide-by-zero exception, isolated). |
| `opcode` | — | Trigger an invalid-opcode exception (isolated to the process). |
| `printmem` | `<hex-addr>` | Dump 32 bytes from an address. |
| `tron` | — | Tron light-cycles game (WASD vs IJKL). |
| `benchmark` | — | Run performance benchmarks. |
| `exit` | — | Report that the shell cannot exit (it keeps the system alive). |

### Course tests (run as user processes)

| Test | Parameters | Description |
|---|---|---|
| `test_mm` | `<bytes>` | Request and free random blocks, checking they do not overlap. |
| `test_proc` | `<max_processes>` | Create, block, unblock, and kill dummy processes at random. |
| `test_prio` | `<value>` | 3 processes incrementing a variable; run with equal and with different priorities. |
| `test_sync` | `<n> <use_sem>` | Pairs of processes increment/decrement a global variable; with semaphores the final value is 0. |

The tests only print on error (except `test_sync`'s final value).

## Special characters and keyboard shortcuts

- `command &` — run `command` in the **background** (`&` must be separated by
  spaces). Without `&`, the command runs in the foreground and the shell waits.
- `p1 | p2` — connect the output of `p1` to the input of `p2` through a **pipe**
  (`|` must be separated by spaces). Example: `cat | wc`.
- **Ctrl+C** — kill the foreground process(es) (does not affect the shell).
- **Ctrl+D** — send end of file (EOF) to the process reading from the keyboard.
- **Ctrl+R** — capture the register snapshot (view it with `inforeg`).
- **Ctrl + / Ctrl -** — increase / decrease the font size.

## Examples by requirement

These demonstrate each requirement with ordinary commands, separately from the
course tests (which are listed last).

```text
# Memory manager
mem                       # heap state (total, used, free, blocks, allocs/frees)

# Processes and scheduling (create / list / block / kill)
loop &                    # one loop in the background
loop &                    # another
ps                        # both loops + shell (sh) + idle are listed
block <pid>               # block a loop; block <pid> again unblocks it
kill <pid>                # kill it

# Priority is observable (a higher-priority process gets the CPU more often)
loop &                    # leave a loop running in the background
nice <pid> 4              # raise its priority (1..4); it now prints far more often

# IPC: pipes (terminal/pipe transparency)
cat | wc                  # type lines, Ctrl+D -> "Lines: N"
cat | filter              # type text, Ctrl+D -> text without vowels
ps | wc                   # one command's output consumed by another

# Synchronization with semaphores (non-test demonstration)
mvar 2 2                  # writers/readers coordinate over a 1-slot cell using
                          # two semaphores: never two values at once, no race.
                          # 2 writers (A,B), 2 readers (colors); mixed output
nice <writer_pid> 4       # that writer starts to dominate the output
kill <writer_pid>         # its letter disappears; the other one dominates

# CPU exceptions (each faults in an isolated process; the kernel kills only that
# process, prints which one faulted, and the shell keeps running)
div 1 0                   # divide-by-zero (exception 0)
opcode                    # invalid opcode (exception 6)

# Running the course tests
test_mm 1000000 &         # memory manager stress (background)
test_proc 5 &             # process stress (background)
test_prio 1000000000      # priority effect (use a large value so it is visible)
test_sync 100 1           # with semaphores -> "Final value: 0"
test_sync 100 0           # without semaphores -> varying value (race condition)
```

## Missing or partially implemented requirements

All requirements in the assignment are implemented and verified. Every command
— including the ones inherited from the Architecture TP (`tron`, `benchmark`, …)
and the utilities (`help`, `time`, `clear`, …) — runs as a process; there are no
built-ins.

## Limitations

- **Heap explicitly requested by a process**: a process's own blocks (stacks,
  argv copy, FPU area) are always recycled, whether it exits normally or is
  killed; when a process dies its zombie children are reaped and its living
  children are re-parented to the shell, so neither leaks. The one case not
  reclaimed is memory a process requested for itself through the `malloc`
  syscall: the kernel does not track which heap blocks belong to which process,
  so if such a process is killed mid-execution that memory is lost (no current
  application uses the `malloc` syscall, so in practice this does not occur).
- **Pipes**: a single pipe per line (`p1 | p2`, not `p1 | p2 | p3`); `|` and `&`
  must be separated by spaces; pipes are not supported in the background.
- **No scroll**: the text console clears the screen on overflow instead of
  scrolling, so a very long dump can push earlier lines off the top.
- The shell (PID 0) shows `STACK BASE 0x0` in `ps` because it runs on the
  kernel's static stack, not on a heap-allocated one.

## Design notes

- The only way processes talk to the kernel is through system calls
  (`syscall`/`sysret`).
- **Priority as frequency**: the scheduler is a deficit round robin where a
  process is eligible `priority` times per round, so a higher-priority process is
  scheduled proportionally more often. This makes priority observable for both
  CPU-bound (`test_prio`) and cooperative, yield-bound workloads (`mvar` with
  `nice`), not only CPU-bound ones.
- **Cooperative yield**: `yield()` triggers an immediate context switch (software
  vector `0x81`) instead of waiting for the next timer tick, so a yielding
  process comes back as soon as the scheduler picks it again.
- The system is free of deadlocks, race conditions, and busy waiting, except for
  the busy waiting the assignment explicitly allows (`loop` and `test_sync`
  without semaphores). Every blocking path (pipes, semaphores, waitpid, keyboard
  read, and the sound driver) sleeps with `_hlt` and is woken by an event or by
  the timer; shared state is guarded by atomic spinlocks and interrupt-safe
  critical sections.
- **Exceptions** (divide-by-zero, invalid opcode) terminate the faulting process
  and hand control back to the scheduler, so the rest of the system keeps
  running; the shell survives instead of the system restarting.
- Synchronization uses spinlocks with the atomic `xchg` instruction. Semaphores
  are named (shared by unrelated processes via an agreed string id) and
  reference-counted (destroyed only on the last `close`).
- The context switch preserves the general-purpose registers and the FPU/SSE
  state (per-process fxsave/fxrstor).
- The reasoning behind these decisions, the bugs found, and the memory model are
  written up in the documents listed in [Documentation](#documentation).

## Documentation

The main documentation for this TP are the two analyses we wrote for the
Operating Systems work (Typst sources under `doc/`, with their compiled PDFs
alongside):

| Document | Location | Contents |
|---|---|---|
| Problem analysis | `doc/problem_analysis.pdf` | Reported issues triaged into real bugs vs. false positives, the deficit-round-robin scheduler (priority as frequency), the cooperative yield, the all-process model, the `mvar` design, and the removal of the last busy-wait. |
| Memory analysis | `doc/memory_analysis.pdf` | An exact per-process heap model (`Used(n) = 50848 + 25184·n`) derived from the code and validated to the byte against `mem`. |

The low-level bugs found while building the kernel and how each was solved are
logged in `AresOS/bugfixes/troubleshooting.md`.

For background, the reports we wrote for the Computer Architecture TP this kernel
extends also live in the repo:
`doc/computer_architecture_documents/DevelopmentReport.pdf` (design and
implementation) and `UserManual.pdf` (end-user manual).

The `.typ` sources sit next to each PDF; rebuild any of them with
`typst compile <file>.typ`.

## Code citations and use of AI

### Third-party and course-provided code

This kernel is built on top of the Computer Architecture TP base project and
retains a few external components, each kept with its own license/attribution:

- **Pure64** (`AresOS/Bootloader/Pure64`) — third-party x86-64 bootloader used to
  bring the CPU into long mode (see `Bootloader/Pure64/docs/LICENSE.TXT`).
- **BMFS** (`AresOS/Bootloader/BMFS`) — third-party BareMetal File System used to
  lay out the boot image (see `Bootloader/BMFS/README.md`).
- The course-provided **test skeletons** `test_mm`, `test_proc` and the shared
  `test_util` helpers (`AresOS/Userland/UserCodeModule/tests/`).

Every kernel subsystem (scheduler, processes, semaphores, pipes, both memory
managers, exceptions) and the whole userland (shell and applications) were
written by the group. Any code fragment adapted from an external source is
attributed in a comment at its use site.

### Use of artificial intelligence

AI (Claude, by Anthropic) was used during development as an assistant, mainly
for:

- Diagnosing low-level bugs.
- Code review and quality suggestions.
- Writing documentation.

Design decisions, verification, and integration were done by the group. The
technical detail of the assisted problems is in
`AresOS/bugfixes/troubleshooting.md` and `doc/problem_analysis.pdf`.

## Project structure

```
Ares/
├── AresOS/
│   ├── Bootloader/   # Pure64 + BMFS
│   ├── Kernel/       # core (processes, scheduler, semaphores, pipes),
│   │                 # lib (memory managers, slab), drivers, arch/x86_64
│   ├── Userland/     # shell, applications, libc, course tests
│   ├── Toolchain/    # ModulePacker
│   ├── Image/        # bootable image generation
│   └── bugfixes/     # documentation of solved bugs
└── doc/              # assignment and documentation
```

## Git hooks setup (optional, for development)

```bash
./setup-hooks.sh    # sets core.hooksPath to .githooks/ (pre-commit: clang-format)
```

---

Developed as part of academic work at ITBA (Instituto Tecnológico de Buenos
Aires).
