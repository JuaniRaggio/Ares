#!/bin/bash
docker run -d -v /Users/juaniraggio/workspace/itbaworkspace/Ares/x64BareBones:/root --security-opt seccomp:unconfined -ti --name ARES agodio/itba-so:1.0
