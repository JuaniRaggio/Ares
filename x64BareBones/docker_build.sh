#!/bin/bash
sudo docker run -d -v "$(pwd)":/root --security-opt seccomp:unconfined -ti --name ARES agodio/itba-so:1.0

