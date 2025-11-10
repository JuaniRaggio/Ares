Ares Recursive Experimental System - AresOS
===============================================================================

Environment setup:
1. Install the following packages before building the Toolchain and Kernel:

  - nasm
  - qemu
  - gcc 
  - make
  - docker

2. Download docker image

  /Ares/AresOS
  > docker pull agodio/itba-so:2.0

3. Initialize container (inside the Ares/AresOS directory)
  /Ares/AresOS
  > docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -it \
    --name ARES agodio/itba-so:2.0

4. We have both clean and build scripts for this project 
(inside the Ares/AresOS directory)

- Cleanup:
  /Ares/AresOS
  > ./clean_in_container.sh ARES

- Build:
  /Ares/AresOS
  > ./compile_in_container.sh ARES

5. After building the project
(inside the Ares/AresOS directory)

- Run:
  /Ares/AresOS
  > ./run.sh

===============================================================================
