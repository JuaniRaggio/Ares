#!/usr/bin/env python3

import json
import os
import glob

KERNEL_DIR = os.path.dirname(os.path.abspath(__file__))

# Compiler and flags from Makefile.inc
GCC = "x86_64-linux-gnu-gcc"
CFLAGS = [
    "-m64",
    "-fno-exceptions",
    "-fno-asynchronous-unwind-tables",
    "-mno-mmx",
    "-mno-sse",
    "-mno-sse2",
    "-fno-builtin-malloc",
    "-fno-builtin-free",
    "-fno-builtin-realloc",
    "-mno-red-zone",
    "-Wall",
    "-ffreestanding",
    "-nostdlib",
    "-fno-common",
    "-std=c99",
    "-fno-pie",
    "-Iinclude",
    "-Iinclude/arch/x86_64",
    "-Iinclude/core",
    "-Iinclude/drivers",
    "-Idrivers/video",
]

def find_c_files():
    """Find all .c files in the kernel directories"""
    c_files = []
    for root, dirs, files in os.walk(KERNEL_DIR):
        # Skip build directory
        if 'build' in root or 'bin' in root:
            continue
        for file in files:
            if file.endswith('.c'):
                c_files.append(os.path.join(root, file))
    return c_files

def generate_compile_commands():
    """Generate compile_commands.json"""
    compile_commands = []

    c_files = find_c_files()

    for file_path in c_files:
        # Make path relative to kernel directory for cleaner output
        rel_path = os.path.relpath(file_path, KERNEL_DIR)

        entry = {
            "directory": KERNEL_DIR,
            "command": f"{GCC} {' '.join(CFLAGS)} -c {rel_path} -o build/{rel_path}.o",
            "file": file_path
        }
        compile_commands.append(entry)

    # Write to compile_commands.json
    output_path = os.path.join(KERNEL_DIR, "compile_commands.json")
    with open(output_path, 'w') as f:
        json.dump(compile_commands, f, indent=2)

    print(f"Generated compile_commands.json with {len(compile_commands)} entries")
    return output_path

if __name__ == "__main__":
    generate_compile_commands()
