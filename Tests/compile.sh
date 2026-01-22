#!/usr/bin/env bash
set -e

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <file.S|file.c|file.elf> [out.elf]"
  echo "Example: $0 Tests/print_file.c /tmp/test.elf"
  exit 2
fi

INPUT="$1"
OUT="${2:-}"

make_tmp_out() {
  mkdir -p /tmp/sim/
  echo "/tmp/sim/test.elf"
}

if [[ -z "$OUT" ]]; then
  OUT="$(make_tmp_out)"
fi

case "$INPUT" in
  *.S)
    riscv64-linux-gnu-gcc -march=rv32i -mabi=ilp32 -nostdlib -static -Wl,--entry=_start \
      -o "$OUT" "$INPUT"
    ;;
  *.c)
    riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -static -specs=picolibc.specs \
      -nostartfiles -o "$OUT" "$INPUT" Tests/c/picolibc_syscalls.c
    ;;
  *.elf)
    OUT=$INPUT
    ;;
  *)
    echo "Unsupported input: $INPUT" >&2
    exit 2
    ;;
esac

echo "$OUT"
