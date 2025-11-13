# ARES: A Recursive Experimental System

<!-- <p align="center"> -->
<!--   <img src="assets/ares_godlike.webp" alt="ITBA" width="500"> -->
<!-- </p> -->

## Overview

ARES (A Recursive Experimental System) is an experimental operating system developed for the Intel x64 architecture. This project implements a complete OS environment including a bootloader, kernel, and userland applications, built upon the x64BareBones foundation.

The project demonstrates core operating system concepts including:
- Low-level hardware initialization and bootloading
- Kernel design and implementation
- System call interfaces
- Process and memory management
- User space program execution

## Getting Started

For detailed instructions on building and running the operating system, please refer to the [`Readme.txt`](AresOS/Readme.txt) file located in the `AresOS` directory.

## Development Environment Setup

### Prerequisites

- `docker`: Required to download the image with all the required tools
- `clang-format`: Required for automatic code formatting
- `nasm`: Assembler for x86-64
- `qemu`: Emulator for testing the OS
- `gcc`: GNU Compiler Collection
- `make`: Build automation tool

### Configuring Git Hooks

This project uses git hooks to maintain code consistency and quality. To set up the hooks in your local repository:

```bash
./setup-hooks.sh
```

This command configures git to use the hooks in the `.githooks/` directory, which includes:

- **pre-commit**: Automatically applies `clang-format` to all C/C++ files before committing

## Project Structure

```
Ares/
├── AresOS/          # Main operating system implementation
│   ├── Bootloader/  # System bootloader
│   ├── Kernel/      # OS kernel
│   ├── Userland/    # User space applications
│   ├── Toolchain/   # Build toolchain
│   └── Image/       # Boot image generation
├── doc/             # Documentation
└── assets/          # Project assets
```

## License

This project is developed as part of academic work at ITBA (Instituto Tecnológico de Buenos Aires).

