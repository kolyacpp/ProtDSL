.section .text
.globl _start
_start:
    addi   x1,   x0,    1
    addi   x2,   x0,    0
    addi   x3,   x0,    1
    addi   x4,   x0,   10
loop:
    beq    x4,   x1, done
    add    x6,   x2,   x0
    add    x2,   x3,   x0
    add    x3,   x2,   x6
    addi   x4,   x4,   -1
    beq    x0,   x0, loop
done:
    add    x0,   x0,   x3

    li a0, 0
    li a7, 93
    ecall