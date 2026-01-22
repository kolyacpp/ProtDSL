	.section .text
	.globl _start

_start:
	# LUI/AUIPC
	lui x1, %hi(test_data)
	addi x1, x1, %lo(test_data)
	auipc x2, 0

	# R-type
	add  x3, x1, x2
	sub  x4, x3, x1
	sll  x5, x3, x4
	slt  x6, x4, x3
	sltu x7, x4, x3
	xor  x8, x6, x7
	srl  x9, x8, x6
	sra  x10, x8, x6
	or   x11, x9, x10
	and  x12, x9, x10

	# I-type
	addi  x13, x12, 5
	slti  x14, x13, 10
	sltiu x15, x13, 10
	xori  x16, x13, 0x55
	ori   x17, x13, 0xaa
	andi  x18, x13, 0x0f
	slli  x19, x13, 1
	srli  x20, x13, 1
	srai  x21, x13, 1

	# B
	addi x22, x0, 1
	addi x23, x0, 1
	beq  x22, x23, 1f
	addi x24, x0, 0
1:
	bne  x22, x23, 2f
	addi x24, x0, 1
2:
	blt  x0, x22, 3f
	addi x24, x0, 2
3:
	bge  x22, x0, 4f
	addi x24, x0, 3
4:
	bltu x0, x22, 5f
	addi x24, x0, 4
5:
	bgeu x22, x0, 6f
	addi x24, x0, 5
6:

	# J
	jal  x25, jal_target
	addi x26, x0, 0
jal_return:
	addi x27, x0, 7
	jal  x0, after_jal

jal_target:
	addi x26, x0, 1
	jalr x0, x25, 0

after_jal:
	# Stores
	addi x28, x0, 0x11
	sb   x28, 0(x1)
	lui  x28, 0x2
	addi x28, x28, 0x233
	sh   x28, 2(x1)
	lui  x28, 0x44556
	addi x28, x28, 0x677
	sw   x28, 4(x1)

	# Loads
	lb   x29, 0(x1)
	lbu  x30, 0(x1)
	lh   x31, 2(x1)
	lhu  x5,  2(x1)
	lw   x6,  4(x1)

	# Fences
	fence
	.option arch, rv32i_zifencei
	fence.i
	.option arch, rv32i

	# syscall
	addi x10, x0, 0
	addi x17, x0, 93 # exit
	ecall

	.section .data
	.align 4
test_data:
	.word 0
	.word 0
	.word 0
