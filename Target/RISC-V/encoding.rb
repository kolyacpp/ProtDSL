require_relative "../../Generic/base"

module SimInfra
    def format_u(opcode, rd, imm)
        return :U, [
            field(rd.name, 11, 7, :reg),
            field(imm.name, 31, 12, :imm),
            field(:opcode, 6, 0, opcode),
        ]
    end

    def format_r(opcode, funct3, funct7, rd, rs1, rs2)
        return :R, [
            field(rd.name, 11, 7, :reg),
            field(rs1.name, 19, 15, :reg),
            field(rs2.name, 24, 20, :reg),
            field(:opcode, 6, 0, opcode),
            field(:funct7, 31, 25, funct7),
            field(:funct3, 14, 12, funct3),
        ]
    end

    def format_i(opcode, funct3, rd, rs1, imm)
        return :I, [
            field(rd.name, 11, 7, :reg),
            field(rs1.name, 19, 15, :reg),
            field(imm.name, 31, 20, :imm),
            field(:opcode, 6, 0, opcode),
            field(:funct3, 14, 12, funct3),
        ]
    end

    def format_i_shift(opcode, funct3, funct7, rd, rs1, imm)
        return :I, [
            field(rd.name, 11, 7, :reg),
            field(rs1.name, 19, 15, :reg),
            field(imm.name, 24, 20, :imm),
            field(:opcode, 6, 0, opcode),
            field(:funct3, 14, 12, funct3),
            field(:funct7, 31, 25, funct7),
        ]
    end

    def format_s(opcode, funct3, rs1, rs2, imm)
        return :S, [
            field(rs1.name, 19, 15, :reg),
            field(rs2.name, 24, 20, :reg),
            immpart(imm.name, 11, 7, 4, 0),
            immpart(imm.name, 31, 25, 11, 5),
            field(:opcode, 6, 0, opcode),
            field(:funct3, 14, 12, funct3),
        ]
    end

    def format_b(opcode, funct3, rs1, rs2, imm)
        return :B, [
            field(rs1.name, 19, 15, :reg),
            field(rs2.name, 24, 20, :reg),
            immpart(imm.name, 11, 8, 4, 1),
            immpart(imm.name, 30, 25, 10, 5),
            immpart(imm.name, 7, 7, 11, 11),
            immpart(imm.name, 31, 31, 12, 12),
            field(:opcode, 6, 0, opcode),
            field(:funct3, 14, 12, funct3),
        ]
    end

    def format_j(opcode, rd, imm)
        return :J, [
            field(rd.name, 11, 7, :reg),
            immpart(imm.name, 19, 12, 19, 12),
            immpart(imm.name, 20, 20, 11, 11),
            immpart(imm.name, 30, 21, 10, 1),
            immpart(imm.name, 31, 31, 20, 20),
            field(:opcode, 6, 0, opcode),
        ]
    end

    def format_r_alu(name, rd, rs1, rs2)
        map = {
            add:  [0b000, 0b0000000],
            sub:  [0b000, 0b0100000],
            sll:  [0b001, 0b0000000],
            slt:  [0b010, 0b0000000],
            sltu: [0b011, 0b0000000],
            xor:  [0b100, 0b0000000],
            srl:  [0b101, 0b0000000],
            sra:  [0b101, 0b0100000],
            or:   [0b110, 0b0000000],
            and:  [0b111, 0b0000000],
        }
        assert(map.key?(name), "Unknown R-type ALU op: #{name}")
        funct3, funct7 = map[name]
        format_r(0b0110011, funct3, funct7, rd, rs1, rs2)
    end

    def format_i_alu(name, rd, rs1, imm)
        map = {
            addi:  0b000,
            slti:  0b010,
            sltiu: 0b011,
            xori:  0b100,
            ori:   0b110,
            andi:  0b111,
        }
        assert(map.key?(name), "Unknown I-type ALU op: #{name}")
        format_i(0b0010011, map[name], rd, rs1, imm)
    end

    def format_i_shift_alu(name, rd, rs1, imm)
        map = {
            slli: [0b001, 0b0000000],
            srli: [0b101, 0b0000000],
            srai: [0b101, 0b0100000],
        }
        assert(map.key?(name), "Unknown I-type shift op: #{name}")
        funct3, funct7 = map[name]
        format_i_shift(0b0010011, funct3, funct7, rd, rs1, imm)
    end

    def format_branch(name, rs1, rs2, imm)
        map = {
            beq:  0b000,
            bne:  0b001,
            blt:  0b100,
            bge:  0b101,
            bltu: 0b110,
            bgeu: 0b111,
        }
        assert(map.key?(name), "Unknown branch op: #{name}")
        format_b(0b1100011, map[name], rs1, rs2, imm)
    end

    def format_load(name, rd, rs1, imm)
        map = {
            lb:  0b000,
            lh:  0b001,
            lw:  0b010,
            lbu: 0b100,
            lhu: 0b101,
        }
        assert(map.key?(name), "Unknown load op: #{name}")
        format_i(0b0000011, map[name], rd, rs1, imm)
    end

    def format_store(name, rs1, rs2, imm)
        map = {
            sb: 0b000,
            sh: 0b001,
            sw: 0b010,
        }
        assert(map.key?(name), "Unknown store op: #{name}")
        format_s(0b0100011, map[name], rs1, rs2, imm)
    end

    def format_u_imm(name, rd, imm)
        map = {
            lui:   0b0110111,
            auipc: 0b0010111,
        }
        assert(map.key?(name), "Unknown U-type op: #{name}")
        format_u(map[name], rd, imm)
    end

    def format_jal(rd, imm)
        format_j(0b1101111, rd, imm)
    end

    def format_jalr(rd, rs1, imm)
        format_i(0b1100111, 0b000, rd, rs1, imm)
    end
end