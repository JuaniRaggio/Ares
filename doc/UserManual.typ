#set document(
  title: "ARES - User Manual",
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

#set list(indent: 1em, marker: (""", "æ", "ª"))
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
  #text(size: 20pt, weight: "bold")[USER MANUAL]
  #v(0.5em)
  #text(size: 18pt)[ARES - ARES Recursive Experimental System] \
  #text(size: 14pt, fill: gray)[Complete Guide for Building, Running, and Using the OS]
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

// Introduction
= Introduction

== About ARES

ARES (ARES Recursive Experimental System) is an educational operating system kernel developed from scratch for the x86-64 architecture. This project demonstrates fundamental concepts of computer architecture, low-level programming, and operating system design.

The system features:
- Custom bootloader based on Pure64
- Kernel running in 64-bit long mode
- Hardware drivers (video, keyboard, timer, sound)
- Interactive shell with built-in commands
- Tron game as a demonstration application

== Document Purpose

This manual provides comprehensive instructions for:
1. Building and compiling the ARES operating system
2. Running the OS in QEMU emulator
3. Managing the development environment
4. Using the shell and its commands
5. Playing the Tron game

#pagebreak()

= Building and Compiling

== Prerequisites

Before building ARES, ensure you have the following tools installed:

=== Required Software

- *Docker*: Container runtime for the build environment
- *Bash*: Shell scripting (usually pre-installed on Unix-like systems)
- *QEMU*: x86-64 system emulator for running the OS
- *Git*: Version control (for managing the repository)

#important[
All compilation must be done inside a Docker container to ensure a consistent build environment with the correct toolchain (GCC cross-compiler for x86-64 bare metal).
]

=== Installing Docker

```bash
# macOS
brew install docker

# Ubuntu/Debian
sudo apt-get install docker.io

# Arch Linux
sudo pacman -S docker
```

=== Installing QEMU

```bash
# macOS
brew install qemu

# Ubuntu/Debian
sudo apt-get install qemu-system-x86

# Arch Linux
sudo pacman -S qemu
```

== Compilation Scripts

=== compile_in_container.sh

This script compiles the project inside a Docker container.

*Usage:*

```bash
./AresOS/compile_in_container.sh [CONTAINER_NAME] [TARGET]
```

*Parameters:*
- `CONTAINER_NAME`: Name of the Docker container (default: `ARES`)
- `TARGET`: Make target to build (default: `all`)

*Examples:*

```bash
# Compile everything using default container
./AresOS/compile_in_container.sh

# Compile using a specific container
./AresOS/compile_in_container.sh my-ares-container

# Compile only the kernel
./AresOS/compile_in_container.sh ARES kernel

# Compile only userland
./AresOS/compile_in_container.sh ARES userland
```

*What it does:*

1. Verifies Docker is installed and running
2. Checks if the specified container exists
3. Starts the container if it's not running
4. Executes `make [TARGET]` inside the container
5. Fixes file permissions after compilation
6. Reports success or failure

#note[
The script automatically corrects file permissions of generated files, as Docker containers often create files with root ownership.
]

=== clean_in_container.sh

This script cleans all build artifacts from the project.

*Usage:*

```bash
./AresOS/clean_in_container.sh [CONTAINER_NAME]
```

*Parameters:*
- `CONTAINER_NAME`: Name of the Docker container (default: `ARES`)

*Examples:*

```bash
# Clean using default container
./AresOS/clean_in_container.sh

# Clean using specific container
./AresOS/clean_in_container.sh my-ares-container
```

*What it does:*

This script is a wrapper around `compile_in_container.sh` that executes the `clean` make target. It removes:
- Object files (`.o`)
- Binary files
- Build directories
- Generated images
- Temporary files

#tip[
Always run `clean_in_container.sh` before making significant changes to the build system or when switching between different development branches.
]

== Build Targets

The Makefile supports several targets:

=== all (default)

Builds the complete system: kernel, userland, and bootable image.

```bash
./AresOS/compile_in_container.sh ARES all
```

=== kernel

Builds only the kernel binary.

```bash
./AresOS/compile_in_container.sh ARES kernel
```

=== userland

Builds only the userland applications (shell and programs).

```bash
./AresOS/compile_in_container.sh ARES userland
```

=== clean

Removes all build artifacts.

```bash
./AresOS/clean_in_container.sh
```

== Build Output

After successful compilation, you will find:

- `AresOS/Image/x64BareBonesImage.img`: Raw bootable disk image
- `AresOS/Image/x64BareBonesImage.qcow2`: QEMU copy-on-write disk image
- `AresOS/Kernel/bin/`: Kernel binaries
- `AresOS/Kernel/build/`: Kernel object files
- `AresOS/Userland/*/bin/`: Userland binaries

#pagebreak()

= Running the Operating System

== Using the Run Script

The simplest way to run ARES is using the provided run script.

*Usage:*

```bash
./AresOS/run.sh [-d]
```

*Options:*
- `-d`: Run in debug mode (waits for GDB connection)

*Examples:*

```bash
# Run normally
./AresOS/run.sh

# Run in debug mode
./AresOS/run.sh -d
```

#note[
The script automatically selects the QCOW2 image if available, otherwise it uses the raw IMG file.
]

== Manual QEMU Invocation

You can also run QEMU manually with custom options:

```bash
# Basic run with 512MB RAM
qemu-system-x86_64 -hda Image/x64BareBonesImage.img -m 512

# Run with specific resolution
qemu-system-x86_64 -hda Image/x64BareBonesImage.img -m 512 \
  -vga std

# Run with KVM acceleration (Linux only)
qemu-system-x86_64 -hda Image/x64BareBonesImage.img -m 512 \
  -enable-kvm

# Run with serial output
qemu-system-x86_64 -hda Image/x64BareBonesImage.img -m 512 \
  -serial stdio
```

== Debugging with GDB

To debug the kernel with GDB:

1. Start QEMU in debug mode:
```bash
./AresOS/run.sh -d
```

2. In another terminal, connect GDB:
```bash
gdb AresOS/Kernel/bin/kernel.elf
(gdb) target remote localhost:1234
(gdb) continue
```

#tip[
Add breakpoints before running `continue`:
```
(gdb) break kernel_main
(gdb) continue
```
]

#pagebreak()

= Project Structure Validation

== validate_structure.sh

This script validates the project's directory structure and file organization.

*Usage:*

```bash
./AresOS/validate_structure.sh
```

*What it checks:*

1. *Duplicate Headers*: Ensures no duplicate header files exist
2. *Duplicate Drivers*: Checks for duplicate driver implementations
3. *Relative Includes*: Verifies no relative path includes (e.g., `#include "../file.h"`)
4. *Compile Flags*: Confirms `compile_flags.txt` files exist
5. *Compile Commands*: Checks for `compile_commands.json` (for IDE support)
6. *Directory Structure*: Validates all required directories are present

*Example output:*

```
===================================
Validating project structure
===================================

[1] Checking for duplicate headers...
  OK: No duplicate headers found

[2] Checking for duplicate driver files...
  OK: No duplicate files found

[3] Checking for relative path includes...
  OK: No relative path includes found

[4] Checking compile_flags.txt existence...
  OK: Kernel/compile_flags.txt exists
  OK: Userland/UserCodeModule/compile_flags.txt exists

[5] Checking compile_commands.json existence...
  OK: Kernel/compile_commands.json exists

[6] Checking directory structure...
  OK: All required directories exist

===================================
VALIDATION SUCCESSFUL
===================================
```

#important[
Always run this validation script after making structural changes to the project or before submitting code.
]

#pagebreak()

= Git Hooks Management

Git hooks help maintain code quality by automatically running checks before commits.

== install-hooks.sh

This script installs Git pre-commit hooks that automatically format code.

*Usage:*

```bash
./install-hooks.sh
```

*What it does:*

1. Verifies you're in a Git repository
2. Checks if `clang-format` is installed
3. Creates a pre-commit hook that:
   - Detects staged `.c` and `.h` files
   - Automatically formats them using `clang-format`
   - Re-stages the formatted files
   - Commits the formatted code

*Installing clang-format:*

```bash
# macOS
brew install clang-format

# Ubuntu/Debian
sudo apt-get install clang-format

# Arch Linux
sudo pacman -S clang
```

#note[
Each developer must run this script on their local machine. Git hooks are not automatically shared via the repository.
]

== setup-hooks.sh

This script configures Git to use a custom hooks directory.

*Usage:*

```bash
./setup-hooks.sh
```

*What it does:*

1. Verifies the repository structure
2. Configures Git to use the `.githooks` directory
3. Lists available hooks
4. Displays confirmation message

*Example output:*

```
Configuring git hooks for the project Ares...
Configuration successful!
Git will now use hooks in the .githooks/ directory

Available hooks:
pre-commit

The hooks will run automatically on corresponding git operations.
```

#tip[
After running this script, any hooks in `.githooks/` will execute automatically during Git operations like commit, push, etc.
]

#pagebreak()

= Shell Commands Reference

Once ARES boots, you will be presented with an interactive shell prompt:

```
ARES>
```

This section describes all available commands.

== help

Displays a list of all available commands with brief descriptions.

*Syntax:*
```
help
```

*Parameters:* None

*Example output:*
```
Available commands:
  help: List all available commands
  man: Show manual for a specific command
  inforeg: Display captured CPU registers
  time: Show system elapsed time
  div: Integer division of two numbers
  clear: Clear the entire screen
  printmem: Memory dump of 32 bytes from an address
  history: Show command history
  exit: Exit Ares OS
  cursor: Change cursor shape (block, hollow, line, underline)
  tron: Play the Tron game (WASD vs IJKL)
```

== man

Shows detailed manual information for a specific command.

*Syntax:*
```
man <command>
```

*Parameters:*
- `command`: Name of the command to get information about

*Example:*
```
ARES> man div
Command: div
Description: Integer division of two numbers
Parameters: 2
```

== inforeg

Displays a snapshot of all CPU registers at the moment of the last register capture.

*Syntax:*
```
inforeg
```

*Parameters:* None

*Example output:*
```
===== Register snapshot: =====
      RIP: 0x100234
      RSP: 0x9ffe0
      RAX: 0x0
      RBX: 0x0
      RCX: 0xa0000
      RDX: 0x3
      RBP: 0x9fff8
      RDI: 0x200000
      RSI: 0x100000
      R8:  0x0
      R9:  0x0
      R10: 0x0
      R11: 0x0
      R12: 0x0
      R13: 0x0
      R14: 0x0
      R15: 0x0
```

#note[
Register values are captured through a syscall that takes a snapshot of the current register state.
]

== time

Shows the current system time and elapsed time since shell startup.

*Syntax:*
```
time
```

*Parameters:* None

*Example output:*
```
Current time: 14:32:15
Time elapsed: 0:5:23
```

#note[
Time is read from the Real-Time Clock (RTC). The elapsed time is calculated from when the shell was first started.
]

== div

Performs integer division of two numbers.

*Syntax:*
```
div <numerator> <denominator>
```

*Parameters:*
- `numerator`: The number to be divided (dividend)
- `denominator`: The number to divide by (divisor)

*Examples:*
```
ARES> div 42 6
42 / 6 = 7

ARES> div 100 7
100 / 7 = 14

ARES> div 10 0
Error: division by zero
```

#warning[
Division by zero is handled gracefully and will display an error message instead of causing a system exception.
]

== clear

Clears the entire screen and moves the cursor to the top-left position.

*Syntax:*
```
clear
```

*Parameters:* None

*Example:*
```
ARES> clear
[screen clears]
ARES>
```

== printmem

Displays a hexadecimal dump of 32 bytes from a specified memory address.

*Syntax:*
```
printmem <address>
```

*Parameters:*
- `address`: Memory address in hexadecimal format (with or without `0x` prefix)

*Examples:*
```
ARES> printmem 0x100000
Memory at 0x100000:
48 89 e5 48 83 ec 20 48
89 7d f8 48 89 75 f0 48
8b 45 f8 48 8b 55 f0 48
01 d0 48 89 45 e8 48 8b

ARES> printmem 400000
Memory at 0x400000:
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
```

#warning[
Reading from invalid or protected memory addresses may cause the system to display an error or exception.
]

== history

Displays the command history showing previously executed commands.

*Syntax:*
```
history
```

*Parameters:* None

*Example output:*
```
Command history:
1: help
2: time
3: div 42 6
4: clear
5: printmem 0x100000
```

If no commands have been executed:
```
No commands in history
```

== exit

Exits the ARES operating system.

*Syntax:*
```
exit
```

*Parameters:* None

#note[
This will typically halt the system or return to a halt state. In QEMU, you can close the window or use Ctrl+A then X to exit.
]

== cursor

Changes the cursor shape in the shell.

*Syntax:*
```
cursor <shape>
```

*Parameters:*
- `shape`: One of: `block`, `hollow`, `line`, or `underline`

*Examples:*
```
ARES> cursor block
Cursor shape set to: block

ARES> cursor hollow
Cursor shape set to: hollow

ARES> cursor line
Cursor shape set to: line

ARES> cursor underline
Cursor shape set to: underline

ARES> cursor invalid
Invalid cursor type or not supported yet. Current options: block, hollow, line, underline
To add a new cursor type, contact support@ares.com
```

*Cursor Shapes:*
- *block*: Solid filled rectangle
- *hollow*: Rectangle outline only
- *line*: Vertical line
- *underline*: Horizontal line at bottom

== tron

Launches the Tron light cycle game.

*Syntax:*
```
tron
```

*Parameters:* None

*Game Controls:*

Player 1 (Cyan):
- `W`: Move up
- `A`: Move left
- `S`: Move down
- `D`: Move right

Player 2 (Magenta):
- `I`: Move up
- `J`: Move left
- `K`: Move down
- `L`: Move right

*Objective:*

Each player controls a light cycle that leaves a trail behind it. The goal is to force your opponent to crash into a trail (yours or theirs) while avoiding crashes yourself.

*Game Rules:*
1. You cannot reverse direction (e.g., if moving right, you cannot immediately move left)
2. Crashing into any trail results in losing the round
3. Last player standing wins

*To Exit:*

Press `ESC` to return to the shell.

#tip[
Try to cut off your opponent's path while keeping your own escape routes open.
]

#pagebreak()

= Troubleshooting

== Compilation Issues

=== Docker container not found

*Error:*
```
ERROR: The container 'ARES' does not exist
```

*Solution:*
Create a Docker container or use the correct container name.

=== Permission denied errors

*Error:*
```
Permission denied when accessing files in Kernel/build/
```

*Solution:*
The compile script should fix this automatically, but you can manually fix permissions:
```bash
sudo chown -R $(id -u):$(id -g) AresOS/Kernel/build
sudo chown -R $(id -u):$(id -g) AresOS/Image
```

== Runtime Issues

=== QEMU not found

*Error:*
```
qemu-system-x86_64: command not found
```

*Solution:*
Install QEMU using your package manager (see Prerequisites section).

=== Image file not found

*Error:*
```
Could not open 'Image/x64BareBonesImage.img'
```

*Solution:*
Compile the project first:
```bash
./AresOS/compile_in_container.sh
```

=== Black screen on boot

*Possible causes:*
1. QEMU not properly configured for graphics
2. Bootloader not properly installed
3. Kernel binary corrupted

*Solution:*
Try rebuilding:
```bash
./AresOS/clean_in_container.sh
./AresOS/compile_in_container.sh
./AresOS/run.sh
```

== Development Issues

=== clang-format not found

*Error when installing hooks:*
```
Warning: clang-format is not installed
```

*Solution:*
Install clang-format (see Git Hooks section).

=== Validation script fails

*Error:*
```
VALIDATION FAILED: X errors found
```

*Solution:*
Review the error messages and fix the reported issues. Common problems:
- Duplicate header files
- Relative path includes
- Missing directories
- Missing `compile_flags.txt` files

#pagebreak()

= Quick Reference

== Common Command Sequences

*Build and run:*
```bash
./AresOS/compile_in_container.sh
./AresOS/run.sh
```

*Clean and rebuild:*
```bash
./AresOS/clean_in_container.sh
./AresOS/compile_in_container.sh
```

*Validate and build:*
```bash
./AresOS/validate_structure.sh
./AresOS/compile_in_container.sh
```

*Setup development environment:*
```bash
./install-hooks.sh
./setup-hooks.sh
```

== Keyboard Shortcuts

*In QEMU:*
- `Ctrl+Alt+G`: Release mouse capture
- `Ctrl+Alt+F`: Toggle fullscreen
- `Ctrl+A`, then `X`: Exit QEMU
- `Ctrl+A`, then `C`: Switch to QEMU console

*In Shell:*
- `Tab`: (Future) Command completion
- `Up/Down`: (Future) Command history navigation
- `Ctrl+C`: (Future) Interrupt current operation

#pagebreak()

= Appendix

== File Structure Overview

```
Ares/
   AresOS/
      Bootloader/          # Pure64 bootloader
      Kernel/              # Kernel source code
         arch/            # Architecture-specific code
         core/            # Core kernel functionality
         drivers/         # Hardware drivers
         include/         # Kernel headers
         Makefile         # Kernel build configuration
      Userland/            # User space programs
         UserCodeModule/  # Shell and applications
             src/         # Source files
             include/     # User headers
             libc/        # C library implementation
      Image/               # Built disk images
      compile_in_container.sh
      clean_in_container.sh
      validate_structure.sh
      run.sh
   doc/                     # Documentation
      UserManual.typ       # This document
      planning.typ         # Project planning
      roadmap.typ          # Development roadmap
   install-hooks.sh
   setup-hooks.sh
```

== System Specifications

*Architecture:* x86-64 (64-bit)

*Bootloader:* Pure64

*Memory:* 512 MB RAM (configurable)

*Display:* VESA graphics mode
- Supported resolutions: 1024x768, 1366x768, 1024x600
- Color depth: 24/32 bpp

*Drivers:*
- Video (framebuffer)
- Keyboard (PS/2)
- Timer/RTC
- PC Speaker (sound)

== Contact and Support

For issues, questions, or contributions:

*Repository:* [Project repository URL]

*Team:*
- Juan Ignacio Raggio
- Enzo Canelo
- Matias Sanchez

#pagebreak()

= Glossary

*Bare Metal:* Code that runs directly on hardware without an operating system underneath.

*Bootloader:* The first program that runs when a computer starts, responsible for loading the operating system.

*Docker Container:* An isolated environment that packages an application and its dependencies.

*Framebuffer:* A portion of memory containing a bitmap that drives a video display.

*QEMU:* Quick Emulator, a generic machine emulator and virtualizer.

*Pure64:* A 64-bit bootloader designed for x86-64 systems.

*RTC:* Real-Time Clock, hardware that keeps track of current time.

*Syscall:* System call, a programmatic way for programs to request services from the kernel.

*x86-64:* 64-bit version of the x86 instruction set architecture.

#align(center)[
  #v(2em)
  #text(size: 14pt, weight: "bold")[End of User Manual]
  #v(1em)
  #text(size: 10pt, fill: gray)[
    For updates and additional information, \
    refer to the project documentation
  ]
]
