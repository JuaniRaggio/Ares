Ares Recursive Experimental System - AresOS
===============================================================================

Environment setup:
1. Install the following packages before building the Toolchain and Kernel:

  - nasm
  - qemu
  - gcc 
  - make
  - docker

2. Download docker image (the one required by the assignment)

  /Ares/AresOS
  > docker pull agodio/itba-so-multiarch:3.1

3. Initialize container (inside the Ares/AresOS directory)
  /Ares/AresOS
  > docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -it \
    --name ARES agodio/itba-so-multiarch:3.1

4. Build / clean (inside the Ares/AresOS directory)

- Build (default memory manager):
  /Ares/AresOS
  > ./compile_in_container.sh ARES

- Build with the buddy system allocator:
  /Ares/AresOS
  > ./compile_in_container.sh ARES buddy

- Cleanup:
  /Ares/AresOS
  > ./compile_in_container.sh ARES clean

5. After building the project
(inside the Ares/AresOS directory)

- Run:
  /Ares/AresOS
  > ./run.sh

===============================================================================

Scripts:
  compile_in_container.sh [CONTAINER] [TARGET] - Build in Docker
  run.sh [-d]                                  - Run in QEMU (-d for debug)

IDE Setup (clangd):
  Run "make compile_commands" on the HOST (not inside docker) to generate
  compile_commands.json with local absolute paths. Re-run it whenever
  source files are added, moved or removed. Shared clangd settings live
  in the .clangd file.

===============================================================================
