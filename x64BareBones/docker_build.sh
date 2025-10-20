#!/bin/bash
docker run -d -v "$(pwd)"/x64BareBones:/root --security-opt seccomp:unconfined -ti --name ARES agodio/itba-so:1.0
