#!/usr/bin/env bash
set -e

cmake --build --preset Release -j12
./build-rel/sim $@
