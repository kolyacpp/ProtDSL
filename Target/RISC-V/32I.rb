require_relative "encoding"
require_relative "regfile"
require_relative "../../Generic/base"

module RV32I
    extend SimInfra

    # U/J-type
    Instruction(:LUI, XReg(:rd), Imm(:imm)) {
        encoding *format_u_imm(:lui, rd, imm)
        asm { "LUI #{rd}, #{imm}" }
        code {
            write(:XRegs, rd, imm << 12)
        }
    }

    Instruction(:AUIPC, XReg(:rd), Imm(:imm)) {
        encoding *format_u_imm(:auipc, rd, imm)
        asm { "AUIPC #{rd}, #{imm}" }
        code {
            write(:XRegs, rd, pc + (imm << 12))
        }
    }

    Instruction(:JAL, XReg(:rd), Imm(:imm)) {
        encoding *format_jal(rd, imm)
        asm { "JAL #{rd}, #{imm}" }
        code {
            write(:XRegs, rd, pc + 4)
            branch(pc + imm)
        }
    }

    Instruction(:JALR, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_jalr(rd, rs1, imm)
        asm { "JALR #{rd}, #{rs1}, #{imm}" }
        code {
            t = pc + 4
            target = (read(:XRegs, rs1) + imm) & -2
            write(:XRegs, rd, t)
            branch(target)
        }
    }

    # I-type
    Instruction(:ADDI, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_i_alu(:addi, rd, rs1, imm)
        asm { "ADDI #{rd}, #{rs1}, #{imm}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) + imm)
        }
    }

    Instruction(:SLTI, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_i_alu(:slti, rd, rs1, imm)
        asm { "SLTI #{rd}, #{rs1}, #{imm}" }
        code {
            a = read(:XRegs, rs1)
            write(:XRegs, rd, select(lt(a, imm), 1, 0))
        }
    }

    Instruction(:SLTIU, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_i_alu(:sltiu, rd, rs1, imm)
        asm { "SLTIU #{rd}, #{rs1}, #{imm}" }
        code {
            a = read(:XRegs, rs1)
            write(:XRegs, rd, select(ltu(a, imm), 1, 0))
        }
    }

    Instruction(:XORI, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_i_alu(:xori, rd, rs1, imm)
        asm { "XORI #{rd}, #{rs1}, #{imm}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) ^ imm)
        }
    }

    Instruction(:ORI, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_i_alu(:ori, rd, rs1, imm)
        asm { "ORI #{rd}, #{rs1}, #{imm}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) | imm)
        }
    }

    Instruction(:ANDI, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_i_alu(:andi, rd, rs1, imm)
        asm { "ANDI #{rd}, #{rs1}, #{imm}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) & imm)
        }
    }

    Instruction(:SLLI, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_i_shift_alu(:slli, rd, rs1, imm)
        asm { "SLLI #{rd}, #{rs1}, #{imm}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) << imm)
        }
    }

    Instruction(:SRLI, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_i_shift_alu(:srli, rd, rs1, imm)
        asm { "SRLI #{rd}, #{rs1}, #{imm}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) >> imm)
        }
    }

    Instruction(:SRAI, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_i_shift_alu(:srai, rd, rs1, imm)
        asm { "SRAI #{rd}, #{rs1}, #{imm}" }
        code {
            write(:XRegs, rd, sar(read(:XRegs, rs1), imm))
        }
    }

    # R-type
    Instruction(:ADD, XReg(:rd), XReg(:rs1), XReg(:rs2)) {
        encoding *format_r_alu(:add, rd, rs1, rs2)
        asm { "ADD #{rd}, #{rs1}, #{rs2}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) + read(:XRegs, rs2))
        }
    }

    Instruction(:SUB, XReg(:rd), XReg(:rs1), XReg(:rs2)) {
        encoding *format_r_alu(:sub, rd, rs1, rs2)
        asm { "SUB #{rd}, #{rs1}, #{rs2}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) - read(:XRegs, rs2))
        }
    }

    Instruction(:SLL, XReg(:rd), XReg(:rs1), XReg(:rs2)) {
        encoding *format_r_alu(:sll, rd, rs1, rs2)
        asm { "SLL #{rd}, #{rs1}, #{rs2}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) << read(:XRegs, rs2))
        }
    }

    Instruction(:SLT, XReg(:rd), XReg(:rs1), XReg(:rs2)) {
        encoding *format_r_alu(:slt, rd, rs1, rs2)
        asm { "SLT #{rd}, #{rs1}, #{rs2}" }
        code {
            a = read(:XRegs, rs1)
            b = read(:XRegs, rs2)
            write(:XRegs, rd, select(lt(a, b), 1, 0))
        }
    }

    Instruction(:SLTU, XReg(:rd), XReg(:rs1), XReg(:rs2)) {
        encoding *format_r_alu(:sltu, rd, rs1, rs2)
        asm { "SLTU #{rd}, #{rs1}, #{rs2}" }
        code {
            a = read(:XRegs, rs1)
            b = read(:XRegs, rs2)
            write(:XRegs, rd, select(ltu(a, b), 1, 0))
        }
    }

    Instruction(:XOR, XReg(:rd), XReg(:rs1), XReg(:rs2)) {
        encoding *format_r_alu(:xor, rd, rs1, rs2)
        asm { "XOR #{rd}, #{rs1}, #{rs2}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) ^ read(:XRegs, rs2))
        }
    }

    Instruction(:SRL, XReg(:rd), XReg(:rs1), XReg(:rs2)) {
        encoding *format_r_alu(:srl, rd, rs1, rs2)
        asm { "SRL #{rd}, #{rs1}, #{rs2}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) >> read(:XRegs, rs2))
        }
    }

    Instruction(:SRA, XReg(:rd), XReg(:rs1), XReg(:rs2)) {
        encoding *format_r_alu(:sra, rd, rs1, rs2)
        asm { "SRA #{rd}, #{rs1}, #{rs2}" }
        code {
            write(:XRegs, rd, sar(read(:XRegs, rs1), read(:XRegs, rs2)))
        }
    }

    Instruction(:OR, XReg(:rd), XReg(:rs1), XReg(:rs2)) {
        encoding *format_r_alu(:or, rd, rs1, rs2)
        asm { "OR #{rd}, #{rs1}, #{rs2}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) | read(:XRegs, rs2))
        }
    }

    Instruction(:AND, XReg(:rd), XReg(:rs1), XReg(:rs2)) {
        encoding *format_r_alu(:and, rd, rs1, rs2)
        asm { "AND #{rd}, #{rs1}, #{rs2}" }
        code {
            write(:XRegs, rd, read(:XRegs, rs1) & read(:XRegs, rs2))
        }
    }

    # B-type
    Instruction(:BEQ, XReg(:rs1), XReg(:rs2), Imm(:imm)) {
        encoding *format_branch(:beq, rs1, rs2, imm)
        asm { "BEQ #{rs1}, #{rs2}, #{imm}" }
        code {
            a = read(:XRegs, rs1)
            b = read(:XRegs, rs2)
            branch(select(eq(a, b), pc + imm, pc + 4))
        }
    }

    Instruction(:BNE, XReg(:rs1), XReg(:rs2), Imm(:imm)) {
        encoding *format_branch(:bne, rs1, rs2, imm)
        asm { "BNE #{rs1}, #{rs2}, #{imm}" }
        code {
            a = read(:XRegs, rs1)
            b = read(:XRegs, rs2)
            branch(select(ne(a, b), pc + imm, pc + 4))
        }
    }

    Instruction(:BLT, XReg(:rs1), XReg(:rs2), Imm(:imm)) {
        encoding *format_branch(:blt, rs1, rs2, imm)
        asm { "BLT #{rs1}, #{rs2}, #{imm}" }
        code {
            a = read(:XRegs, rs1)
            b = read(:XRegs, rs2)
            branch(select(lt(a, b), pc + imm, pc + 4))
        }
    }

    Instruction(:BGE, XReg(:rs1), XReg(:rs2), Imm(:imm)) {
        encoding *format_branch(:bge, rs1, rs2, imm)
        asm { "BGE #{rs1}, #{rs2}, #{imm}" }
        code {
            a = read(:XRegs, rs1)
            b = read(:XRegs, rs2)
            branch(select(ge(a, b), pc + imm, pc + 4))
        }
    }

    Instruction(:BLTU, XReg(:rs1), XReg(:rs2), Imm(:imm)) {
        encoding *format_branch(:bltu, rs1, rs2, imm)
        asm { "BLTU #{rs1}, #{rs2}, #{imm}" }
        code {
            a = read(:XRegs, rs1)
            b = read(:XRegs, rs2)
            branch(select(ltu(a, b), pc + imm, pc + 4))
        }
    }

    Instruction(:BGEU, XReg(:rs1), XReg(:rs2), Imm(:imm)) {
        encoding *format_branch(:bgeu, rs1, rs2, imm)
        asm { "BGEU #{rs1}, #{rs2}, #{imm}" }
        code {
            a = read(:XRegs, rs1)
            b = read(:XRegs, rs2)
            branch(select(geu(a, b), pc + imm, pc + 4))
        }
    }

    # Loads
    Instruction(:LB, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_load(:lb, rd, rs1, imm)
        asm { "LB #{rd}, #{imm}(#{rs1})" }
        code {
            addr = read(:XRegs, rs1) + imm
            write(:XRegs, rd, mem_read(addr, :b8, :s8))
        }
    }

    Instruction(:LH, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_load(:lh, rd, rs1, imm)
        asm { "LH #{rd}, #{imm}(#{rs1})" }
        code {
            addr = read(:XRegs, rs1) + imm
            write(:XRegs, rd, mem_read(addr, :b16, :s16))
        }
    }

    Instruction(:LW, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_load(:lw, rd, rs1, imm)
        asm { "LW #{rd}, #{imm}(#{rs1})" }
        code {
            addr = read(:XRegs, rs1) + imm
            write(:XRegs, rd, mem_read(addr, :b32, :s32))
        }
    }

    Instruction(:LBU, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_load(:lbu, rd, rs1, imm)
        asm { "LBU #{rd}, #{imm}(#{rs1})" }
        code {
            addr = read(:XRegs, rs1) + imm
            write(:XRegs, rd, mem_read(addr, :b8, :u8))
        }
    }

    Instruction(:LHU, XReg(:rd), XReg(:rs1), Imm(:imm)) {
        encoding *format_load(:lhu, rd, rs1, imm)
        asm { "LHU #{rd}, #{imm}(#{rs1})" }
        code {
            addr = read(:XRegs, rs1) + imm
            write(:XRegs, rd, mem_read(addr, :b16, :u16))
        }
    }

    # Stores
    Instruction(:SB, XReg(:rs2), XReg(:rs1), Imm(:imm)) {
        encoding *format_store(:sb, rs1, rs2, imm)
        asm { "SB #{rs2}, #{imm}(#{rs1})" }
        code {
            addr = read(:XRegs, rs1) + imm
            mem_write(addr, read(:XRegs, rs2), :b8)
        }
    }

    Instruction(:SH, XReg(:rs2), XReg(:rs1), Imm(:imm)) {
        encoding *format_store(:sh, rs1, rs2, imm)
        asm { "SH #{rs2}, #{imm}(#{rs1})" }
        code {
            addr = read(:XRegs, rs1) + imm
            mem_write(addr, read(:XRegs, rs2), :b16)
        }
    }

    Instruction(:SW, XReg(:rs2), XReg(:rs1), Imm(:imm)) {
        encoding *format_store(:sw, rs1, rs2, imm)
        asm { "SW #{rs2}, #{imm}(#{rs1})" }
        code {
            addr = read(:XRegs, rs1) + imm
            mem_write(addr, read(:XRegs, rs2), :b32)
        }
    }

    # System
    Instruction(:ECALL) {
        encoding :E, [field(:c, 31, 0, 0b00000000000000000000000001110011)]
        asm { "ECALL" }
        code { syscall }
    }

    Instruction(:EBREAK) {
        encoding :E, [field(:c, 31, 0, 0b00000000000100000000000001110011)]
        asm { "EBREAK" }
        code { ebreak }
    }

    Instruction(:FENCE) {
        encoding :E, [
            field(:fm, 31, 28, 0b0000),
            field(:pred, 27, 24),
            field(:succ, 23, 20),
            field(:c, 19, 0, 0b00000000000000001111)
        ]
        asm { "FENCE" }
        code { fence }
    }

    Instruction(:FENCE_I) {
        encoding :E, [field(:c, 31, 0, 0b00000000000000000001000000001111)]
        asm { "FENCE.I" }
        code { fence }
    }
end