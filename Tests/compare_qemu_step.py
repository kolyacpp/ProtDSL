#!/usr/bin/env python3
import re
import sys
from pathlib import Path

REG_ALIASES = {
    "zero": 0,
    "ra": 1,
    "sp": 2,
    "gp": 3,
    "tp": 4,
    "t0": 5,
    "t1": 6,
    "t2": 7,
    "s0": 8,
    "fp": 8,
    "s1": 9,
    "a0": 10,
    "a1": 11,
    "a2": 12,
    "a3": 13,
    "a4": 14,
    "a5": 15,
    "a6": 16,
    "a7": 17,
    "s2": 18,
    "s3": 19,
    "s4": 20,
    "s5": 21,
    "s6": 22,
    "s7": 23,
    "s8": 24,
    "s9": 25,
    "s10": 26,
    "s11": 27,
    "t3": 28,
    "t4": 29,
    "t5": 30,
    "t6": 31,
}


def parse_sim(path: Path):
    entries = []
    pc_re = re.compile(r"^pc = (\d+)")
    reg_re = re.compile(r"x(\d+)=(-?\d+)")
    current = None

    for line in path.read_text().splitlines():
        m = pc_re.match(line.strip())
        if m:
            if current is not None:
                entries.append(current)
            current = {"pc": int(m.group(1)), "regs": [0] * 32}
            continue
        if current is None:
            continue
        for r, v in reg_re.findall(line):
            current["regs"][int(r)] = int(v)

    if current is not None:
        entries.append(current)
    return entries


def parse_qemu_step(path: Path):
    entries = []
    pc_re = re.compile(r"^ pc\s+([0-9a-fA-F]+)")
    reg_re = re.compile(r"(x\d+|[a-z][a-z0-9]+)\s+([0-9a-fA-F]+)")

    current = None
    for line in path.read_text().splitlines():
        m = pc_re.match(line)
        if m:
            if current is not None:
                entries.append(current)
            current = {"pc": int(m.group(1), 16), "regs": [0] * 32}
            continue
        if current is None:
            continue
        for r, v in reg_re.findall(line):
            if r.startswith("x"):
                try:
                    idx = int(r[1:])
                except ValueError:
                    continue
            else:
                idx = REG_ALIASES.get(r)
            if idx is None or idx >= 32:
                continue
            current["regs"][idx] = int(v, 16)

    if current is not None:
        entries.append(current)
    return entries


def compare(sim_entries, qemu_entries, ignore_regs=None, skip=0):
    ignore_regs = set(ignore_regs or [])
    mismatches = []

    qemu_by_pc = {}
    for q in qemu_entries:
        qemu_by_pc.setdefault(q["pc"], []).append(q)
    qemu_idx = {pc: 0 for pc in qemu_by_pc}

    for i in range(skip, len(sim_entries)):
        s = sim_entries[i]
        q_list = qemu_by_pc.get(s["pc"])
        if not q_list:
            continue
        idx = qemu_idx[s["pc"]]
        if idx >= len(q_list):
            continue
        q = q_list[idx]
        qemu_idx[s["pc"]] = idx + 1

        for r in range(32):
            if r in ignore_regs:
                continue
            if s["regs"][r] != q["regs"][r]:
                mismatches.append(
                    (i, f"x{r} mismatch at pc {s['pc']}: sim={s['regs'][r]} qemu={q['regs'][r]}")
                )

    if not mismatches:
        print("OK: traces match")
        return 0

    print(f"MISMATCHES ({len(mismatches)})")
    for step, detail in mismatches:
        print(f"Mismatch at step {step}: {detail}")
    return 1


def main():
    if len(sys.argv) < 3:
        print("Usage: compare_qemu_step.py <sim_log> <qemu_step_log> [--ignore x2,x3] [--skip N]")
        sys.exit(2)

    sim_log = Path(sys.argv[1])
    qemu_log = Path(sys.argv[2])
    ignore_regs = []
    skip = 0
    args = sys.argv[3:]
    while args:
        arg = args.pop(0)
        if arg == "--ignore" and args:
            vals = args.pop(0).split(",")
            for v in vals:
                v = v.strip()
                if v.startswith("x"):
                    v = v[1:]
                if v:
                    ignore_regs.append(int(v))
        elif arg == "--skip" and args:
            skip = int(args.pop(0))
        else:
            print(f"Unknown arg: {arg}")
            sys.exit(2)

    sim_entries = parse_sim(sim_log)
    qemu_entries = parse_qemu_step(qemu_log)
    sys.exit(compare(sim_entries, qemu_entries, ignore_regs=ignore_regs, skip=skip))


if __name__ == "__main__":
    main()
