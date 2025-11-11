#!/usr/bin/env bash
cd Kernel
python3 generate_compile_commands.py
cd ../Userland
python3 generate_compile_commands.py
