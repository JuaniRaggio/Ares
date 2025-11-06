#set document(
  title: "ARES - Development Report",
  author: ("Juan Ignacio Raggio", "Enzo Canelo", "Matias Sanchez"),
)

#set page(
  paper: "a4",
  margin: (
    top: 2.5cm,
    bottom: 2.5cm,
    left: 2cm,
    right: 2cm,
  ),
  numbering: "1",
  number-align: bottom + right,

  header: [
    #set text(size: 9pt, fill: gray)
    #grid(
      columns: (1fr, 1fr),
      align: (left, right),
      [Raggio, Canelo, Sanchez],
      [#datetime.today().display("[day]/[month]/[year]")]
    )
    #line(length: 100%, stroke: 0.5pt + gray)
  ],

  footer: context [
    #set text(size: 9pt, fill: gray)
    #line(length: 100%, stroke: 0.5pt + gray)
    #v(0.2em)
    #align(center)[
      Page #counter(page).display() of #counter(page).final().first()
    ]
  ]
)

#set text(
  font: "New Computer Modern",
  size: 11pt,
  lang: "en",
  hyphenate: true,
)

#set par(
  justify: true,
  leading: 0.65em,
  first-line-indent: 0em,
  spacing: 1.2em,
)

#set heading(numbering: "1.1")
#show heading.where(level: 1): set text(size: 16pt, weight: "bold")
#show heading.where(level: 2): set text(size: 14pt, weight: "bold")
#show heading.where(level: 3): set text(size: 12pt, weight: "bold")

#show heading: it => {
  v(0.5em)
  it
  v(0.3em)
}

#set list(indent: 1em, marker: ("•", "◦", "▪"))
#set enum(indent: 1em, numbering: "1.a.")

#show raw.where(block: false): box.with(
  fill: luma(240),
  inset: (x: 3pt, y: 0pt),
  outset: (y: 3pt),
  radius: 2pt,
)

#show raw.where(block: true): block.with(
  fill: luma(240),
  inset: 10pt,
  radius: 4pt,
  width: 100%,
)

#show link: underline

// Helper functions
#let note(content) = {
  block(
    fill: rgb("#E3F2FD"),
    stroke: rgb("#1976D2") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#1976D2"))[Note:] #content
  ]
}

#let important(content) = {
  block(
    fill: rgb("#FFF3E0"),
    stroke: rgb("#F57C00") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#F57C00"))[Important:] #content
  ]
}

#let tip(content) = {
  block(
    fill: rgb("#E8F5E9"),
    stroke: rgb("#388E3C") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#388E3C"))[Tip:] #content
  ]
}

#let warning(content) = {
  block(
    fill: rgb("#FFEBEE"),
    stroke: rgb("#D32F2F") + 1pt,
    inset: 10pt,
    radius: 4pt,
    width: 100%,
  )[
    #text(weight: "bold", fill: rgb("#D32F2F"))[Warning:] #content
  ]
}

// Cover page
#align(center)[
  #v(2em)
  #image("ITBAgua.png", width: 40%)
  #v(1em)
  #text(size: 24pt, weight: "bold")[Computer Architecture]
  #v(0.5em)
  #text(size: 20pt, weight: "bold")[DEVELOPMENT REPORT]
  #v(0.5em)
  #text(size: 18pt)[ARES - ARES Recursive Experimental System] \
  #text(size: 14pt, fill: gray)[Technical Implementation and Architecture Documentation]
  #v(1em)
  #text(size: 12pt)[
    *Authors:* \
    Juan Ignacio Raggio \
    Enzo Canelo \
    Matias Sanchez
  ]
  #v(0.5em)
  #text(size: 12pt, fill: gray)[
    Second Semester 2025 \
    #datetime.today().display("[day]/[month]/[year]")
  ]
  #v(2em)
]

#line(length: 100%, stroke: 1pt)
#v(1em)

#pagebreak()

// Table of contents
#outline(
  title: [Table of Contents],
  indent: auto,
  depth: 3
)

#pagebreak()

= Introduction

== Project Overview

ARES (ARES Recursive Experimental System) is an educational operating system developed for the x86-64 architecture. This report documents the technical implementation details of the Kernel and Userland components that were developed on top of the x64BareBones baseline project.

The x64BareBones project provided the foundational bootloader and basic system initialization. Our team's primary contribution consisted of implementing a complete kernel with system call infrastructure, interrupt handling, exception management, and a fully-featured userland environment with an interactive shell and applications.

== Scope of Implementation

#important[
Our implementation focused exclusively on the Kernel and Userland components. The bootloader (Pure64) and basic hardware initialization were provided by the x64BareBones baseline. This allowed us to concentrate on operating system design principles, privilege level transitions, and user-kernel communication mechanisms.
]

The key areas of our development include:

- *Kernel Architecture*: Complete reorganization and implementation of kernel subsystems
- *System Call Interface*: Modern SYSCALL/SYSRET mechanism for x86-64
- *Interrupt and Exception Handling*: Comprehensive IDT setup and handler implementation
- *Userland Environment*: Shell, C library, and user applications
- *Driver Development*: Video, keyboard, timer, and RTC drivers

== Development Philosophy

Throughout this project, we prioritized two fundamental principles:

*Code Quality Over Product Features*: We deliberately chose to write clean, well-structured, and maintainable code rather than implementing a large number of features. This decision makes the codebase more understandable, easier to debug, and significantly simpler to extend in the future.

*Extensibility and Modularity*: Every subsystem was designed with future expansion in mind. The modular architecture allows new system calls, drivers, and shell commands to be added with minimal changes to existing code. This design philosophy is evident in our use of function tables, clean abstractions, and separation of concerns.

#pagebreak()

= Directory Structure Reorganization

== Original x64BareBones Structure

The x64BareBones baseline provided a simple, flat directory structure suitable for a minimal kernel. This structure works well for small projects but becomes difficult to maintain as complexity grows. All kernel C files resided in the root directory, and the include directory had no organization by subsystem.

== AresOS Reorganized Structure

We significantly reorganized the project to improve maintainability, scalability, and logical separation of concerns. This approach forced us to change almost all Makefiles provided and the build system, even though this brought us some problems, the mantainablility and organization of the proyect improved significantly

```
AresOS/
├── Bootloader/           # Unchanged from baseline
├── Kernel/
│   ├── arch/             # Architecture-specific code
│   │   └── x86_64/
│   │       └── interrupts/
│   │           ├── interrupts.asm
│   │           ├── syscalls.c
│   │           └── exceptions.c
│   ├── asm/              # Low-level assembly
│   │   ├── init_syscalls.asm
│   │   ├── syscalls.asm
│   │   └── syscalls_table.asm
│   ├── core/             # Core kernel functionality
│   ├── drivers/          # Hardware drivers
│   │   ├── keyboard/
│   │   ├── timer/
│   │   └── video/
│   ├── include/          # Headers organized by subsystem
│   │   ├── arch/
│   │   │   └── x86_64/
│   │   ├── core/
│   │   └── drivers/
│   ├── lib/              # Kernel utility libraries
│   └── build/            # Build artifacts (organized)
└── Userland/
    └── UserCodeModule/
        ├── asm/          # User-level assembly (syscall stubs)
        ├── include/      # User headers
        ├── libc/         # C library implementation
        └── src/          # Application source code
            ├── shell.c
            ├── commands.c
            └── ...
```

== Benefits of Reorganization

The reorganized structure provides several significant advantages:

=== Separation of Concerns

Each directory has a clear, well-defined purpose. Architecture-specific code is isolated in `arch/x86_64/`, making it straightforward to add support for other architectures in the future. Drivers are organized by device type, and core kernel functionality is separated from hardware-specific code.

=== Improved Maintainability

When searching for code related to a specific subsystem (e.g., interrupts, keyboard driver, system calls), its easier to know exactly where to look. This organization makes the project significantly easier to understand

=== Scalability

As new features are added, they have clear homes in the directory structure. Adding a new driver means creating a new subdirectory under `drivers/`. Adding a new architecture means creating a new subdirectory under `arch/`.

=== Build System Clarity

The organized structure allows for cleaner, more maintainable Makefiles. Dependencies are easier to track, and partial builds of specific subsystems become straightforward, so if an asm file is added to the project, its trivial to understand which Makefile should be changing

#pagebreak()

= Interesting Implementation Techniques

== Structs in Assembly

One of the notable techniques employed in AresOS is the use of structured data definitions directly in assembly language. This approach provides type safety and clarity when working with complex register snapshots and stack frames.

=== Register Snapshot Structure

In `AresOS/Kernel/arch/x86_64/interrupts/interrupts.asm`, we define a structured layout for CPU register snapshots:

```nasm
struc regs
    _rip: resq 1
    _rsp: resq 1
    _rax: resq 1
    _rbx: resq 1
    _rcx: resq 1
    _rdx: resq 1
    _rbp: resq 1
    _rdi: resq 1
    _rsi: resq 1
    _r8:  resq 1
    _r9:  resq 1
    _r10: resq 1
    _r11: resq 1
    _r12: resq 1
    _r13: resq 1
    _r14: resq 1
    _r15: resq 1
    _cs:  resq 1
    _rflags: resq 1
    _ss:  resq 1
endstruc
```

This structure is then used in exception handlers to save register state:

```nasm
exceptionHandler 1
    cli
    pushState

    ; Save general purpose registers to struct
    mov QWORD [regs + _rax], rax
    mov QWORD [regs + _rbx], rbx
    mov QWORD [regs + _rcx], rcx
    ; ... (additional registers)

    ; Call C exception dispatcher
    mov rdi, %1                  ; Exception number
    lea rsi, [regs]              ; Pointer to register struct
    call exceptionDispatcher
```

=== Advantages of Assembly Structs

Using structs in assembly provides several benefits:

- *Type Safety*: Field offsets are calculated automatically by the assembler, eliminating manual offset calculations and reducing human errors and unreadable code
- *Maintainability*: Adding or reordering fields requires changes only to the struct definition, you don't need to initialize the structure in order
- *Interoperability*: The struct layout matches the corresponding C struct, ensuring seamless data exchange between assembly and C code
- *Readability*: Code using named fields (`regs + _rax`) is much clearer than magic number offsets

#pagebreak()

== Unions for Shell Command Dispatch

The shell implementation demonstrates sophisticated use of C union types to create a flexible, type-safe command dispatch system that can handle commands with varying numbers of parameters. This approach is inspired by lambda functions in Java

=== Command Function Abstraction

In `AresOS/Userland/UserCodeModule/include/commands.h`, we define a union that can hold function pointers with different signatures:

```c
typedef enum {
    supplier_t = 0,      // No parameters
    function_t,          // One parameter
    bi_function_t,       // Two parameters
    max_parameters,
} function_type;

typedef union {
    int (*supplier)(void);              // Commands like 'help', 'clear'
    int (*function)(char *);            // Commands like 'man <cmd>'
    int (*bi_function)(char *, char *); // Commands like 'div <a> <b>'
} executable_t;

typedef struct {
    executable_t execute;
    function_type ftype;
} lambda_t;
```

Each command is then defined with its appropriate function type:

```c
static const command_t div_command = {
    .name        = "div",
    .description = "Integer division of two numbers",
    .lambda = {
        .execute.bi_function = (void *)&div_cmd,
        .ftype               = bi_function_t,
    },
};

static const command_t help_command = {
    .name        = "help",
    .description = "List all available commands",
    .lambda = {
        .execute.supplier = (void *)&help,
        .ftype            = supplier_t,
    },
};
```

=== Type-Safe Dispatch Mechanism

This design enables type-safe runtime dispatch based on the number of parameters. The shell can validate parameter counts and call the appropriate function with compile-time type checking:

```c
// Pseudo-code for command execution
switch (cmd->lambda.ftype) {
    case supplier_t:
        result = cmd->lambda.execute.supplier();
        break;
    case function_t:
        result = cmd->lambda.execute.function(arg1);
        break;
    case bi_function_t:
        result = cmd->lambda.execute.bi_function(arg1, arg2);
        break;
}
```

=== Benefits of Union-Based Dispatch

This approach provides several advantages over traditional function pointer tables:

- *Extensibility*: Adding commands with different parameter counts requires no changes to the dispatch logic
- *Type Safety*: The compiler ensures that each function pointer matches its declared signature
- *Self-Documenting*: The function type explicitly declares how many parameters a command expects
- *No Variable-parameter Functions*: Avoids the type unsafety and runtime overhead of functions with variable parameters

#pagebreak()

= Userland-Kernel Communication

#important[
The communication mechanism between Userland and Kernel is the key of our operating system architecture. We invested significant research and development effort into designing a robust, efficient, and modern system call interface. We found out the usage of the `syscall` instruction over the `int 80h` is most commonly used in 64-bits systems and the `int 80h` provided by the course is mostly used in 32-bits systems
]

== System Call Mechanism: SYSCALL vs INT 0x80h

One of the most critical architectural decisions in AresOS was choosing between the traditional `INT 0x80h` software interrupt mechanism and the modern `SYSCALL` instruction for implementing system calls in 64-bit mode.

=== Research and Comparison

We conducted extensive research comparing both mechanisms across multiple dimensions:

==== INT 0x80h (Software Interrupt)

*Advantages:*
- Universal compatibility across x86 and x86-64
- Simpler conceptual model (just another interrupt)
- Familiar to developers from 32-bit systems
- Main approach used during the course

*Disadvantages:*
- Significant performance overhead
  - Full interrupt dispatch through IDT
  - Stack switching through TSS
  - Segment register checks and validation
- Requires IDT entry setup
- Not optimized for 64-bit mode
- Approximately ~300-1000+ CPU cycles per invocation (source: Linux kernel documentation on syscall performance)

==== SYSCALL/SYSRET (Fast System Call)

*Advantages:*
- Designed specifically for x86-64 long mode
- Minimal overhead (approximately 50-150 cycles)
- Direct jump to kernel handler without IDT lookup
- Automatic privilege level switching
- Hardware-optimized stack and register management
- Uses Model-Specific Registers (MSRs) for configuration

*Disadvantages:*
- Requires explicit MSR initialization
- x86-64 specific (not portable to 32-bit)
- Slightly more complex initial setup
- This approach was not covered in the course, so unexpected issues may arise

=== Decision: SYSCALL for AresOS

After this analysis, we chose SYSCALL/SYSRET for the following reasons:

1. *Performance*: In a 64-bit operating system, system calls are frequent operations. The 2-3x performance improvement translates to better overall system responsiveness.

2. *Modern Design*: SYSCALL is the intended mechanism for system calls in 64-bit mode. Using it aligns with modern operating system design principles.

3. *Cleaner Semantics*: SYSCALL has well-defined register conventions specifically for system calls, unlike INT which is a general-purpose interrupt mechanism.

4. *Future-Proof*: Modern operating systems (Linux, BSD, Windows) all use SYSCALL on x86-64, making our implementation more aligned with industry standards.

#note[
While SYSCALL is x86-64 specific, this is not a limitation for AresOS, which targets exclusively the x86-64 architecture. If portability to 32-bit systems becomes a requirement, a separate INT 0x80h implementation could be added alongside the SYSCALL interface but the outstanding project structure makes this changes simple.
]

#pagebreak()

== SYSCALL Implementation Details

Our SYSCALL implementation consists of three main components: MSR initialization, userland stubs, and kernel handlers.

=== MSR Initialization

The SYSCALL mechanism requires configuring three Model-Specific Registers. This initialization happens during kernel boot in `AresOS/Kernel/asm/init_syscalls.asm`:

```nasm
init_syscalls:
    ; 1. Enable SYSCALL in EFER (Extended Feature Enable Register)
    mov ecx, 0xC0000080          ; IA32_EFER
    rdmsr
    or eax, 1                    ; Bit 0 = SCE (System Call Extensions)
    wrmsr

    ; 2. STAR: Configure segment selectors
    ; Bits 63:48 = User CS (0x1B for ring 3)
    ; Bits 47:32 = Kernel CS (0x08 for ring 0)
    mov ecx, 0xC0000081          ; IA32_STAR
    xor eax, eax                 ; Bits 31:0 = 0 (unused)
    mov edx, 0x00180008          ; EDX = [User CS : Kernel CS]
    wrmsr

    ; 3. LSTAR: Set handler address (64-bit)
    mov ecx, 0xC0000082          ; IA32_LSTAR
    lea rax, [rel syscall_entry] ; Handler address
    mov rdx, rax
    shr rdx, 32                  ; RDX = bits 63:32
    wrmsr

    ; 4. SFMASK: Set RFLAGS mask (optional)
    mov ecx, 0xC0000084
    xor eax, eax
    xor edx, edx
    wrmsr

    ret
```

This initialization:
- Enables SYSCALL support in the CPU
- Configures automatic segment selector switching (ring 3 to ring 0)
- Sets the kernel entry point for all system calls
- Optionally masks RFLAGS bits during transition

=== Userland System Call Stub

User applications invoke system calls through a simple assembly stub in `AresOS/Userland/UserCodeModule/asm/syscall.asm`:

```nasm
global _syscall3

section .text
_syscall3:
    ; Arguments (System V AMD64 ABI):
    ; RDI = syscall number
    ; RSI = arg1
    ; RDX = arg2
    ; RCX = arg3

    mov rax, rdi      ; Move syscall number to RAX
    mov rdi, rsi      ; Shift arg1 to RDI
    mov rsi, rdx      ; Shift arg2 to RSI
    mov rdx, rcx      ; Shift arg3 to RDX
    syscall           ; Enter kernel
    ret               ; Return with result in RAX
```

This stub follows the System V AMD64 calling convention for user functions but adapts it to the SYSCALL register convention (syscall number in RAX, arguments in RDI, RSI, RDX).

#pagebreak()

=== Kernel System Call Entry Point

When a SYSCALL instruction executes, control transfers to the kernel entry point in `AresOS/Kernel/asm/syscalls.asm`:

```nasm
syscall_entry:
    swapgs                      ; Switch to kernel GS (get kernel data)
    mov r15, rsp                ; Save user stack pointer
    mov rsp, kernel_stack_top   ; Switch to kernel stack

    ; Build stack frame
    push r15                    ; Save user RSP
    push rcx                    ; Save user RIP (return address)
    push r11                    ; Save user RFLAGS

    ; Save callee-saved registers
    push rbx
    push rbp
    push r12
    push r13
    push r14

    ; Dispatch to appropriate syscall handler
    mov rbx, rax                ; RBX = syscall number
    lea rcx, [rel syscalls_table]
    shl rbx, 3                  ; rbx * 8 (pointer size)
    add rcx, rbx
    mov rbx, [rcx]              ; Load function pointer
    test rbx, rbx
    jz .unknown_syscall

    ; Arguments already in RDI, RSI, RDX (correct positions)
    call rbx                    ; Call syscall handler
    jmp .done

.unknown_syscall:
    mov rax, -1                 ; Error code

.done:
    ; Restore registers in reverse order
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop r11                     ; Restore user RFLAGS
    pop rcx                     ; Restore user RIP
    pop rsp                     ; Restore user RSP

    swapgs                      ; Switch back to user GS
    sysretq                     ; Return to userland
```

=== SYSCALL Execution Flow

The complete system call flow follows this sequence:

1. *Userland Application* calls a wrapper function (e.g., `write()`)
2. *Wrapper Function* calls `_syscall3` stub with syscall number and arguments
3. *SYSCALL Instruction* triggers privilege level transition (ring 3 → ring 0)
4. *CPU Automatically*:
   - Saves user RIP to RCX
   - Saves user RFLAGS to R11
   - Loads kernel RIP from LSTAR MSR
   - Switches to kernel code segment (from STAR MSR)
5. *Kernel Entry Point* (`syscall_entry`):
   - Switches to kernel stack
   - Saves user context (RSP, RIP, RFLAGS)
   - Saves callee-saved registers
   - Dispatches to appropriate handler
6. *Syscall Handler* executes in kernel mode, returns result in RAX
7. *Kernel Entry Point* restores user context
8. *SYSRETQ Instruction* returns to userland (ring 0 → ring 3)
9. *CPU Automatically*:
   - Restores user RIP from RCX
   - Restores user RFLAGS from R11
   - Switches to user code segment
10. *Userland* continues with result in RAX

#pagebreak()

=== System Call Table

System calls are dispatched through a function pointer table defined in `AresOS/Kernel/asm/syscalls_table.asm`:

```nasm
section .data
global syscalls_table

syscalls_table:
    dq sys_write              ; 0: SYS_WRITE
    dq sys_exit               ; 1: SYS_EXIT
    dq sys_read_wrapper       ; 2: SYS_READ
    dq sys_clear_wrapper      ; 3: SYS_CLEAR
    dq sys_get_ticks_wrapper  ; 4: SYS_GET_TICKS
    dq sys_get_resolution_wrapper ; 5: SYS_GET_RESOLUTION
    dq sys_get_register_array_wrapper ; 6: SYS_GET_REGISTER_ARRAY
    dq sys_set_font_size_wrapper ; 7: SYS_SET_FONT_SIZE
    dq sys_get_memory_wrapper ; 8: SYS_GET_MEMORY
    dq sys_draw_rect_wrapper  ; 9: SYS_DRAW_RECT
    dq sys_get_seconds_wrapper ; 10: SYS_GET_SECONDS
    dq sys_set_text_color_wrapper ; 11: SYS_SET_TEXT_COLOR
    dq sys_set_bg_color_wrapper ; 12: SYS_SET_BG_COLOR
    dq sys_get_cursor_pos_wrapper ; 13: SYS_GET_CURSOR_POS
    dq sys_redraw_screen_wrapper ; 14: SYS_REDRAW_SCREEN
    dq sys_get_time_wrapper ; 15: SYS_GET_TIME
```

This table-driven approach makes adding new system calls trivial: simply add a new entry to the table and define the corresponding handler function.

=== Example System Call Handler

System call handlers are implemented in C. Here's an example from `AresOS/Kernel/arch/x86_64/interrupts/syscalls.c`:

```c
uint64_t sys_read(uint64_t fd, char *buf, uint64_t count) {
    if (fd != 0 || count == 0 || buf == NULL) {
        return 0;
    }
    if (buffer_has_next()) {
        buf[0] = buffer_next();
        return 1;
    }
    return 0;
}

uint64_t sys_write(uint64_t fd, const char *buf, uint64_t len) {
    if (fd != STDOUT && fd != STDERR) {
        return 0;
    }

    uint8_t color = (fd == STDOUT) ?
        current_stdout_color : current_stderr_color;

    for (uint64_t i = 0; i < len; i++) {
        ncPrintChar(buf[i], color);
    }
    return len;
}
```

These handlers have full access to kernel resources and drivers while maintaining clear interfaces for userland applications.

#pagebreak()

== Interrupt and IRQ Handling

In addition to system calls, the kernel must handle hardware interrupts (IRQs) for devices like the keyboard and timer. These use a different mechanism from SYSCALL.

=== Interrupt Descriptor Table (IDT)

Hardware interrupts are handled through the traditional IDT mechanism. Each interrupt vector has a handler registered in the IDT during kernel initialization.

=== IRQ Handler Structure

IRQ handlers follow a standardized pattern defined in `AresOS/Kernel/arch/x86_64/interrupts/interrupts.asm`:

```nasm
%macro irqHandlerMaster 1
    pushState               ; Save all registers

    mov rdi, %1             ; IRQ number as first parameter
    mov rsi, rsp            ; Stack pointer (for register access)
    call irqDispatcher      ; Call C dispatcher

    ; Send EOI (End of Interrupt) to PIC
    mov al, 20h
    out 20h, al

    popState                ; Restore all registers
    iretq                   ; Return from interrupt
%endmacro

; Timer interrupt (IRQ 0)
_irq00Handler:
    irqHandlerMaster 0

; Keyboard interrupt (IRQ 1)
_irq01Handler:
    irqHandlerMaster 1
```

The `pushState` and `popState` macros save and restore all general-purpose registers, ensuring that interrupted code can resume exactly where it left off.

=== IRQ Dispatcher

The assembly handlers call into a C dispatcher that routes interrupts to appropriate device drivers:

```c
void irqDispatcher(uint64_t irq, uint64_t *stack_frame) {
    switch (irq) {
        case 0:  // Timer
            timer_handler();
            break;
        case 1:  // Keyboard
            keyboard_handler();
            break;
        // Additional IRQs...
    }
}
```

This design separates low-level register management (assembly) from high-level device handling logic (C).

#pagebreak()

== Exception Handling

Exceptions represent error conditions or special events during program execution (e.g., division by zero, invalid opcode). AresOS implements comprehensive exception handling to provide useful debugging information.

=== Exception Handler Macro

Exception handlers are defined using a macro that captures complete register state:

```nasm
%macro exceptionHandler 1
    cli                     ; Disable interrupts
    pushState               ; Save all registers

    ; Manually save registers to struct
    mov QWORD [regs + _rax], rax
    mov QWORD [regs + _rbx], rbx
    mov QWORD [regs + _rcx], rcx
    mov QWORD [regs + _rdx], rdx
    ; ... (save all registers)

    ; Registers pushed by CPU are on stack
    mov rax, QWORD [rsp + 15*8]
    mov QWORD [regs + _rip], rax     ; Instruction pointer
    mov rax, QWORD [rsp + 16*8]
    mov QWORD [regs + _cs], rax      ; Code segment
    mov rax, QWORD [rsp + 17*8]
    mov QWORD [regs + _rflags], rax  ; Flags
    mov rax, QWORD [rsp + 18*8]
    mov QWORD [regs + _rsp], rax     ; Stack pointer
    mov rax, QWORD [rsp + 19*8]
    mov QWORD [regs + _ss], rax      ; Stack segment

    ; Call C exception handler
    mov rdi, %1              ; Exception number
    lea rsi, [regs]          ; Pointer to register struct
    call exceptionDispatcher

    popState                 ; Restore registers
    call getStackBase
    mov [rsp+24], rax        ; Reset stack
    mov rax, userland
    mov [rsp], rax           ; Return to userland base

    sti                      ; Re-enable interrupts
    iretq
%endmacro

; Division by zero exception
_exception0Handler:
    exceptionHandler 0

; Invalid opcode exception
_exception6Handler:
    exceptionHandler 6
```

=== Exception Dispatcher and Register Snapshot

The exception dispatcher in `AresOS/Kernel/arch/x86_64/interrupts/exceptions.c` provides detailed information about the exception:

```c
void exceptionDispatcher(int exception, uint64_t *stack_frame) {
    // Extract registers from stack frame
    regs_snapshot_t regs;
    regs.r15    = stack_frame[0];
    regs.r14    = stack_frame[1];
    regs.r13    = stack_frame[2];
    // ... (extract all registers)
    regs.rip    = stack_frame[15];
    regs.rflags = stack_frame[17];
    regs.rsp    = stack_frame[18];

    saved_regs = regs;  // Save for debugging

    if (exception == ZERO_EXCEPTION_ID)
        zero_division(&regs);
    else if (exception == INVALID_OPCODE_ID)
        invalid_opcode(&regs);

    print_registers(&regs);  // Display register state
}
```

When an exception occurs, the user sees:

```
========================================
EXCEPTION 0: DIVISION BY ZERO
A division by zero was attempted.
========================================

REGISTER STATE:
RAX: 0x0000000000000000  RBX: 0x0000000000000000  RCX: 0x00000000000A0000
RDX: 0x0000000000000003  RSI: 0x0000000000100000  RDI: 0x0000000000200000
RBP: 0x0000000000009FFF8  RSP: 0x0000000000009FFE0  RIP: 0x0000000000100234
R8:  0x0000000000000000  R9:  0x0000000000000000  R10: 0x0000000000000000
R11: 0x0000000000000000  R12: 0x0000000000000000  R13: 0x0000000000000000
R14: 0x0000000000000000  R15: 0x0000000000000000
CS:     0x0000000000000008  SS:     0x0000000000000010
RFLAGS: 0x0000000000000202

Press any key to continue...
```

This comprehensive error reporting significantly aids debugging and understanding program failures.

#pagebreak()

= Code Quality and Extensibility

#important[
Throughout the development of AresOS, we prioritized code quality and extensibility over feature completeness. This philosophy reflects our belief that a well-architected, maintainable codebase is more valuable than a feature-rich but poorly structured one.
]

== Emphasis on Code Quality

Our commitment to code quality manifests in several ways throughout the project:

=== Clean Code Principles

- *Meaningful Names*: Functions, variables, and structures use descriptive names that clearly convey their purpose (e.g., `exceptionDispatcher`, `syscall_entry`, `irqHandlerMaster`)
- *Single Responsibility*: Each function performs one well-defined task
- *Consistent Style*: We use automated formatting (clang-format) enforced through Git hooks
- *Comments and Documentation*: Complex algorithms and hardware interactions are thoroughly documented

=== Modular Architecture

The codebase is organized into clear subsystems with well-defined interfaces:

- *Kernel Subsystems*: Interrupts, syscalls, drivers, core functionality
- *Driver Abstraction*: Each driver has a consistent interface
- *Library Functions*: Reusable utilities separated from specific implementations

=== Type Safety

We leverage C's type system to catch errors at compile time:

- Strong typing for function pointers in command dispatch
- Structured definitions for register snapshots
- Enumerated types for syscall numbers and command indices

== Extensibility by Design

Every major component of AresOS was designed to be extended with minimal code changes:

=== Adding New System Calls

Adding a new system call requires only three steps:

1. *Define the syscall number* in `syscalls_numbers.h`:
```c
typedef enum {
    SYS_WRITE = 0,
    SYS_EXIT,
    // ... existing syscalls ...
    SYS_NEW_FEATURE,  // Add here
    SYS_MAX,
} syscall_codes;
```

2. *Implement the handler* in `syscalls.c`:
```c
uint64_t sys_new_feature(uint64_t arg1, uint64_t arg2) {
    // Implementation
    return result;
}
```

3. *Add to syscall table* in `syscalls_table.asm`:
```nasm
syscalls_table:
    dq sys_write
    dq sys_exit
    ; ...
    dq sys_new_feature_wrapper
```

No changes to the syscall entry logic, dispatcher, or userland stub are required.

=== Adding New Shell Commands

The union-based command system makes adding commands equally straightforward:

1. *Implement the command function*:
```c
int my_new_cmd(char *arg) {
    // Implementation
    return 0;
}
```

2. *Define the command structure*:
```c
static const command_t mynew_command = {
    .name        = "mynew",
    .description = "Description of my new command",
    .lambda = {
        .execute.function = (void *)&my_new_cmd,
        .ftype            = function_t,
    },
};
```

3. *Add to command table*:
```c
static const command_t *const commands[QTY_COMMANDS] = {
    &help_command,
    // ... existing commands ...
    &mynew_command,
};
```

The shell automatically handles dispatch, parameter validation, and help text generation.

=== Adding New Exception Handlers

Exception handling is similarly extensible:

1. *Define exception handler in assembly*:
```nasm
_exception13Handler:
    exceptionHandler 13  ; General Protection Fault
```

2. *Implement handler in C*:
```c
static void general_protection_fault(regs_snapshot_t *regs) {
    ncPrint("EXCEPTION 13: GENERAL PROTECTION FAULT\n", VGA_RED);
    // Additional handling
}
```

3. *Add to dispatcher*:
```c
void exceptionDispatcher(int exception, uint64_t *stack_frame) {
    // ...
    else if (exception == 13)
        general_protection_fault(&regs);
}
```

4. *Register in IDT* during initialization.

=== Adding New Drivers

The driver architecture allows new hardware support to be added as self-contained modules:

1. Create driver directory: `drivers/new_device/`
2. Implement standard driver interface:
   - Initialization function
   - IRQ handler (if applicable)
   - Public API functions
3. Register driver during kernel initialization
4. Add to build system

The driver can then be used by both kernel and userland code through system calls.

== Benefits of This Approach

The emphasis on quality and extensibility provides tangible benefits:

=== Easier Debugging

Clean, well-structured code is easier to debug. When an issue arises, developers can quickly:
- Locate the relevant code
- Understand its logic
- Identify the root cause
- Implement a fix

=== Lower Maintenance Cost

Code that is easy to understand requires less time to maintain. Future developers (including our future selves) will spend less time deciphering intent and more time adding value.

=== Educational Value

As an educational project, AresOS serves as a learning resource. Clean, well-documented code is more effective for teaching operating system concepts than complex, optimized code.

#pagebreak()

== Key Achievements

=== Architectural Decisions

We made informed technical decisions based on research and analysis:
- Choosing SYSCALL over INT 0x80h for optimal performance in 64-bit mode
- Reorganizing the directory structure for improved maintainability
- Implementing a modular, extensible architecture

=== Technical Implementation

We successfully implemented core operating system components:
- MSR-based SYSCALL/SYSRET mechanism with proper privilege transitions
- Comprehensive interrupt and exception handling
- Device drivers for keyboard, video, timer, and RTC
- User-space environment with C library and shell

=== Code Quality

We prioritized maintainability and extensibility:
- Clean, well-documented code
- Modular architecture with clear separation of concerns
- Type-safe abstractions using structs and unions
- Extensible design patterns throughout

== Final Thoughts

AresOS demonstrates that prioritizing code quality and architectural soundness creates a more valuable result than rushing to implement features. The clean, extensible codebase we created will serve as an excellent foundation for future development and as an educational resource for understanding operating system implementation.
