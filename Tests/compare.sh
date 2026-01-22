#!/usr/bin/env bash
set -e

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <file.S|file.c|file.elf> [ignore_regs]"
  echo "Example: $0 Tests/fib_bss.S x1,x2,x3,x4,x10,x11,x12"
  exit 2
fi

INPUT="$1"
IGNORE_REGS="${2:-}"

TMP_DIR=/tmp/sim
ELF=$TMP_DIR/test.elf
SIM_STDOUT=$TMP_DIR/sim_stdout.log
SIM_DEBUG=$TMP_DIR/sim_debug.log
QEMU_LOG=$TMP_DIR/qemu_step.log

ELF=$(./Tests/compile.sh $INPUT)

./build-rel/sim -i $ELF --debug > $SIM_STDOUT 2> $SIM_DEBUG

qemu-riscv32 -one-insn-per-tb -d cpu -D $QEMU_LOG $ELF

python3 ./Tests/compare_qemu_step.py $SIM_DEBUG $QEMU_LOG --ignore "$IGNORE_REGS"
