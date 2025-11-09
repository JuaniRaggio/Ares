#!/usr/bin/env python3

import json
import os
import glob

USERLAND_DIR = os.path.dirname(os.path.abspath(__file__))
MODULE_DIR = os.path.join(USERLAND_DIR, "UserCodeModule")

# Compiler and flags from Makefile.inc and UserCodeModule/Makefile
GCC = "x86_64-linux-gnu-gcc"
CFLAGS = [
    "-m64",
    "-fno-exceptions",
    "-std=c99",
    "-Wall",
    "-ffreestanding",
    "-nostdlib",
    "-fno-common",
    "-mno-red-zone",
    "-mno-mmx",
    "-mno-sse",
    "-mno-sse2",
    "-fno-builtin-malloc",
    "-fno-builtin-free",
    "-fno-builtin-realloc",
    "-fno-pie",
    "-no-pie",
    "-Ilibc/include",
    "-Iinclude",
]

def find_c_files():
    """Find all .c files in the UserCodeModule directories"""
    c_files = []
    c_files.extend(glob.glob(os.path.join(MODULE_DIR, "*.c")))
    c_files.extend(glob.glob(os.path.join(MODULE_DIR, "src", "*.c")))
    c_files.extend(glob.glob(os.path.join(MODULE_DIR, "libc", "src", "*.c")))
    return c_files

def generate_compile_commands():
    """Generate compile_commands.json"""
    compile_commands = []

    c_files = find_c_files()

    for file_path in c_files:
        # Make path relative to module directory for cleaner output
        rel_path = os.path.relpath(file_path, MODULE_DIR)

        entry = {
            "directory": MODULE_DIR,
            "command": f"{GCC} {' '.join(CFLAGS)} -c {rel_path} -o build/{os.path.basename(rel_path)}.o",
            "file": file_path
        }
        compile_commands.append(entry)

    # Write to compile_commands.json
    output_path = os.path.join(USERLAND_DIR, "compile_commands.json")
    with open(output_path, 'w') as f:
        json.dump(compile_commands, f, indent=2)

    print(f"Generated compile_commands.json with {len(compile_commands)} entries")
    return output_path

if __name__ == "__main__":
    generate_compile_commands()
