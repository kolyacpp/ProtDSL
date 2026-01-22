#!/usr/bin/env bash
set -e

ruby main.rb

ruby Generator/Decoder.rb IR.yaml generated/decoder.hpp &
ruby Generator/Interpreter.rb IR.yaml generated/interpreter.hpp &
wait
