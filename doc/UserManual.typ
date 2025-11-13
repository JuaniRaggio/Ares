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

#set list(indent: 1em, marker: ([•], [◦], [▪]))
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

=== all

Builds the complete system: kernel, userland, and bootable image.

```bash
./AresOS/compile_in_container.sh
# or
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

- `AresOS/Image/x64BareBonesImage.img`: Raw bootable disk image
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

#note[
You can also run QEMU manually with custom options
]

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

#pagebreak()

= Shell Commands Reference

Once ARES boots, you will be presented with an interactive shell prompt:

```
>
```

This section describes all available commands.

== help

Displays a list of all available commands with brief descriptions.

*Syntax:*
```
help
```

*Parameters:* None

#note[
The commands `history` and `cursor` are not yet implemented in the current version.
]

== man

Shows detailed manual information for a specific command.

*Syntax:*
```
man <command>
```

*Parameters:*
- `command`: Name of the command to get information about

== inforeg

Displays a snapshot of all CPU registers at the moment of the last register capture.

*Syntax:*
```
inforeg
```

*Parameters:* None

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

#note[
Time is read from the Real-Time Clock (RTC), it has a 3 hour gap with Argentina's current time. The elapsed time is calculated from when the shell was first started.
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

#warning[
Division by zero is not handled gracefully so it will cause a system exception.
]

== clear

Clears the entire screen and moves the cursor to the top-left position.

*Syntax:*
```
clear
```

*Parameters:* None

== printmem

Displays a hexadecimal dump of 32 bytes from a specified memory address.

*Syntax:*
```
printmem <address>
```

*Parameters:*
- `address`: Memory address in hexadecimal format (with or without `0x` prefix)

#warning[
Reading from invalid or protected memory addresses may cause the system to have unexpected problems
]

== history

#warning[
This command is not yet implemented in the current version of ARES.
]

Displays the command history showing previously executed commands.

*Syntax:*
```
history
```

*Parameters:* None

== exit

#warning[
We need Scheduler to make this work
]

Exits the ARES operating system

*Syntax:*
```
exit
```

*Parameters:* None

== cursor

#warning[
This command is not yet implemented in the current version of ARES.
]

Changes the cursor shape in the shell.

*Syntax:*
```
cursor <shape>
```

*Parameters:*
- `shape`: One of: `block`, `hollow`, `line`, or `underline`

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

*To Exit:*

Press `ESC` to return to the shell.

== textcolor

Changes the foreground text color of the shell.

*Syntax:*
```
textcolor <color>
```

*Parameters:*
- `color`: Color name or hexadecimal RGB value

*Supported color names:*
- `black`, `white`, `red`, `green`, `blue`
- `yellow`, `cyan`, `magenta`
- `gray`, `lightgray`, `darkgray`

#note[
The text color affects all subsequent text output in STDOUT. The color persists until changed again or the system is restarted.
]

== bgcolor

Changes the background color of the shell.

*Syntax:*
```
bgcolor <color>
```

*Parameters:*
- `color`: Color name or hexadecimal RGB value

*Supported color names:*
- `black`, `white`, `red`, `green`, `blue`
- `yellow`, `cyan`, `magenta`
- `gray`, `lightgray`, `darkgray`

#tip[
Combine `textcolor` and `bgcolor` to create custom color schemes. Use the `clear` command after changing colors to see the full effect.
]

== benchmark

Runs a comprehensive performance benchmark suite testing various system components.

*Syntax:*
```
benchmark
```

*Parameters:* None

*What it tests:*

1. *FPS (Frames Per Second) Benchmark*
2. *Timer Benchmark*
3. *Syscall Read Benchmark*

#note[
The benchmark requires user interaction to advance between tests. Press any key when prompted to continue to the next benchmark.
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

\

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

== System Specifications

*Architecture:* x86-64 (64-bit)

*Bootloader:* Pure64

*Memory:* 512 MB RAM (configurable)

*Drivers:*
- Video (framebuffer)
- Keyboard
- Timer/RTC
- PC Speaker (sound)

