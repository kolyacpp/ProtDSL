#pragma once
#include <cstdint>
#include <optional>
#include <system_error>

enum class InstrId : uint32_t {
    LUI,
    AUIPC,
    JAL,
    JALR,
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,
    ECALL,
    EBREAK,
    FENCE,
    FENCE_I,
    INVALID
};

struct DecodedInstrution {
    InstrId id;
    uint32_t rd;
    uint32_t rs1;
    uint32_t rs2;
    int32_t imm;
};

constexpr uint32_t extract_bits(uint32_t w, uint32_t hi, uint32_t lo) noexcept {
    uint32_t width = hi - lo + 1;
    if (width >= 32) return w;
    return (w >> lo) & ((uint32_t(1) << width) - 1);
}

constexpr int32_t sign_extend(uint32_t x, uint32_t bits) noexcept {
    if (bits == 0) return 0;
    uint32_t m = uint32_t(1) << (bits - 1);
    return int32_t((x ^ m) - m);
}

inline std::optional<DecodedInstrution> decode(uint32_t ins) {
    switch (extract_bits(ins, 6, 0)) {
    case 0b0000011: {
        switch (extract_bits(ins, 14, 12)) {
        case 0b000: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::LB, rd, rs1, rs2, imm};
            break;
        }
        case 0b001: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::LH, rd, rs1, rs2, imm};
            break;
        }
        case 0b010: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::LW, rd, rs1, rs2, imm};
            break;
        }
        case 0b100: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::LBU, rd, rs1, rs2, imm};
            break;
        }
        case 0b101: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::LHU, rd, rs1, rs2, imm};
            break;
        }
        default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
        }
        break;
    }
    case 0b0001111: {
        switch (extract_bits(ins, 19, 7)) {
        case 0b0000000000000: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            int32_t imm = 0;
            return DecodedInstrution{InstrId::FENCE, rd, rs1, rs2, imm};
            break;
        }
        case 0b0000000100000: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            int32_t imm = 0;
            return DecodedInstrution{InstrId::FENCE_I, rd, rs1, rs2, imm};
            break;
        }
        default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
        }
        break;
    }
    case 0b0010011: {
        switch (extract_bits(ins, 14, 12)) {
        case 0b000: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::ADDI, rd, rs1, rs2, imm};
            break;
        }
        case 0b001: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 24, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 5);
            return DecodedInstrution{InstrId::SLLI, rd, rs1, rs2, imm};
            break;
        }
        case 0b010: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::SLTI, rd, rs1, rs2, imm};
            break;
        }
        case 0b011: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::SLTIU, rd, rs1, rs2, imm};
            break;
        }
        case 0b100: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::XORI, rd, rs1, rs2, imm};
            break;
        }
        case 0b101: {
            switch (extract_bits(ins, 31, 25)) {
            case 0b0000000: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                uint32_t imm_raw = 0;
                imm_raw |= (extract_bits(ins, 24, 20) << 0);
                int32_t imm = sign_extend(imm_raw, 5);
                return DecodedInstrution{InstrId::SRLI, rd, rs1, rs2, imm};
                break;
            }
            case 0b0100000: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                uint32_t imm_raw = 0;
                imm_raw |= (extract_bits(ins, 24, 20) << 0);
                int32_t imm = sign_extend(imm_raw, 5);
                return DecodedInstrution{InstrId::SRAI, rd, rs1, rs2, imm};
                break;
            }
            default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
            }
            break;
        }
        case 0b110: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::ORI, rd, rs1, rs2, imm};
            break;
        }
        case 0b111: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rd = extract_bits(ins, 11, 7);
            rs1 = extract_bits(ins, 19, 15);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 31, 20) << 0);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::ANDI, rd, rs1, rs2, imm};
            break;
        }
        default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
        }
        break;
    }
    case 0b0010111: {
        uint32_t rd = 0;
        uint32_t rs1 = 0;
        uint32_t rs2 = 0;
        rd = extract_bits(ins, 11, 7);
        uint32_t imm_raw = 0;
        imm_raw |= (extract_bits(ins, 31, 12) << 0);
        int32_t imm = sign_extend(imm_raw, 20);
        return DecodedInstrution{InstrId::AUIPC, rd, rs1, rs2, imm};
        break;
    }
    case 0b0100011: {
        switch (extract_bits(ins, 14, 12)) {
        case 0b000: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rs1 = extract_bits(ins, 19, 15);
            rs2 = extract_bits(ins, 24, 20);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 11, 7) << 0);
            imm_raw |= (extract_bits(ins, 31, 25) << 5);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::SB, rd, rs1, rs2, imm};
            break;
        }
        case 0b001: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rs1 = extract_bits(ins, 19, 15);
            rs2 = extract_bits(ins, 24, 20);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 11, 7) << 0);
            imm_raw |= (extract_bits(ins, 31, 25) << 5);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::SH, rd, rs1, rs2, imm};
            break;
        }
        case 0b010: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rs1 = extract_bits(ins, 19, 15);
            rs2 = extract_bits(ins, 24, 20);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 11, 7) << 0);
            imm_raw |= (extract_bits(ins, 31, 25) << 5);
            int32_t imm = sign_extend(imm_raw, 12);
            return DecodedInstrution{InstrId::SW, rd, rs1, rs2, imm};
            break;
        }
        default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
        }
        break;
    }
    case 0b0110011: {
        switch (extract_bits(ins, 31, 25)) {
        case 0b0000000: {
            switch (extract_bits(ins, 14, 12)) {
            case 0b000: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                rs2 = extract_bits(ins, 24, 20);
                int32_t imm = 0;
                return DecodedInstrution{InstrId::ADD, rd, rs1, rs2, imm};
                break;
            }
            case 0b001: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                rs2 = extract_bits(ins, 24, 20);
                int32_t imm = 0;
                return DecodedInstrution{InstrId::SLL, rd, rs1, rs2, imm};
                break;
            }
            case 0b010: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                rs2 = extract_bits(ins, 24, 20);
                int32_t imm = 0;
                return DecodedInstrution{InstrId::SLT, rd, rs1, rs2, imm};
                break;
            }
            case 0b011: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                rs2 = extract_bits(ins, 24, 20);
                int32_t imm = 0;
                return DecodedInstrution{InstrId::SLTU, rd, rs1, rs2, imm};
                break;
            }
            case 0b100: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                rs2 = extract_bits(ins, 24, 20);
                int32_t imm = 0;
                return DecodedInstrution{InstrId::XOR, rd, rs1, rs2, imm};
                break;
            }
            case 0b101: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                rs2 = extract_bits(ins, 24, 20);
                int32_t imm = 0;
                return DecodedInstrution{InstrId::SRL, rd, rs1, rs2, imm};
                break;
            }
            case 0b110: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                rs2 = extract_bits(ins, 24, 20);
                int32_t imm = 0;
                return DecodedInstrution{InstrId::OR, rd, rs1, rs2, imm};
                break;
            }
            case 0b111: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                rs2 = extract_bits(ins, 24, 20);
                int32_t imm = 0;
                return DecodedInstrution{InstrId::AND, rd, rs1, rs2, imm};
                break;
            }
            default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
            }
            break;
        }
        case 0b0100000: {
            switch (extract_bits(ins, 14, 12)) {
            case 0b000: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                rs2 = extract_bits(ins, 24, 20);
                int32_t imm = 0;
                return DecodedInstrution{InstrId::SUB, rd, rs1, rs2, imm};
                break;
            }
            case 0b101: {
                uint32_t rd = 0;
                uint32_t rs1 = 0;
                uint32_t rs2 = 0;
                rd = extract_bits(ins, 11, 7);
                rs1 = extract_bits(ins, 19, 15);
                rs2 = extract_bits(ins, 24, 20);
                int32_t imm = 0;
                return DecodedInstrution{InstrId::SRA, rd, rs1, rs2, imm};
                break;
            }
            default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
            }
            break;
        }
        default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
        }
        break;
    }
    case 0b0110111: {
        uint32_t rd = 0;
        uint32_t rs1 = 0;
        uint32_t rs2 = 0;
        rd = extract_bits(ins, 11, 7);
        uint32_t imm_raw = 0;
        imm_raw |= (extract_bits(ins, 31, 12) << 0);
        int32_t imm = sign_extend(imm_raw, 20);
        return DecodedInstrution{InstrId::LUI, rd, rs1, rs2, imm};
        break;
    }
    case 0b1100011: {
        switch (extract_bits(ins, 14, 12)) {
        case 0b000: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rs1 = extract_bits(ins, 19, 15);
            rs2 = extract_bits(ins, 24, 20);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 11, 8) << 1);
            imm_raw |= (extract_bits(ins, 30, 25) << 5);
            imm_raw |= (extract_bits(ins, 7, 7) << 11);
            imm_raw |= (extract_bits(ins, 31, 31) << 12);
            int32_t imm = sign_extend(imm_raw, 13);
            return DecodedInstrution{InstrId::BEQ, rd, rs1, rs2, imm};
            break;
        }
        case 0b001: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rs1 = extract_bits(ins, 19, 15);
            rs2 = extract_bits(ins, 24, 20);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 11, 8) << 1);
            imm_raw |= (extract_bits(ins, 30, 25) << 5);
            imm_raw |= (extract_bits(ins, 7, 7) << 11);
            imm_raw |= (extract_bits(ins, 31, 31) << 12);
            int32_t imm = sign_extend(imm_raw, 13);
            return DecodedInstrution{InstrId::BNE, rd, rs1, rs2, imm};
            break;
        }
        case 0b100: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rs1 = extract_bits(ins, 19, 15);
            rs2 = extract_bits(ins, 24, 20);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 11, 8) << 1);
            imm_raw |= (extract_bits(ins, 30, 25) << 5);
            imm_raw |= (extract_bits(ins, 7, 7) << 11);
            imm_raw |= (extract_bits(ins, 31, 31) << 12);
            int32_t imm = sign_extend(imm_raw, 13);
            return DecodedInstrution{InstrId::BLT, rd, rs1, rs2, imm};
            break;
        }
        case 0b101: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rs1 = extract_bits(ins, 19, 15);
            rs2 = extract_bits(ins, 24, 20);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 11, 8) << 1);
            imm_raw |= (extract_bits(ins, 30, 25) << 5);
            imm_raw |= (extract_bits(ins, 7, 7) << 11);
            imm_raw |= (extract_bits(ins, 31, 31) << 12);
            int32_t imm = sign_extend(imm_raw, 13);
            return DecodedInstrution{InstrId::BGE, rd, rs1, rs2, imm};
            break;
        }
        case 0b110: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rs1 = extract_bits(ins, 19, 15);
            rs2 = extract_bits(ins, 24, 20);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 11, 8) << 1);
            imm_raw |= (extract_bits(ins, 30, 25) << 5);
            imm_raw |= (extract_bits(ins, 7, 7) << 11);
            imm_raw |= (extract_bits(ins, 31, 31) << 12);
            int32_t imm = sign_extend(imm_raw, 13);
            return DecodedInstrution{InstrId::BLTU, rd, rs1, rs2, imm};
            break;
        }
        case 0b111: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            rs1 = extract_bits(ins, 19, 15);
            rs2 = extract_bits(ins, 24, 20);
            uint32_t imm_raw = 0;
            imm_raw |= (extract_bits(ins, 11, 8) << 1);
            imm_raw |= (extract_bits(ins, 30, 25) << 5);
            imm_raw |= (extract_bits(ins, 7, 7) << 11);
            imm_raw |= (extract_bits(ins, 31, 31) << 12);
            int32_t imm = sign_extend(imm_raw, 13);
            return DecodedInstrution{InstrId::BGEU, rd, rs1, rs2, imm};
            break;
        }
        default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
        }
        break;
    }
    case 0b1100111: {
        uint32_t rd = 0;
        uint32_t rs1 = 0;
        uint32_t rs2 = 0;
        rd = extract_bits(ins, 11, 7);
        rs1 = extract_bits(ins, 19, 15);
        uint32_t imm_raw = 0;
        imm_raw |= (extract_bits(ins, 31, 20) << 0);
        int32_t imm = sign_extend(imm_raw, 12);
        return DecodedInstrution{InstrId::JALR, rd, rs1, rs2, imm};
        break;
    }
    case 0b1101111: {
        uint32_t rd = 0;
        uint32_t rs1 = 0;
        uint32_t rs2 = 0;
        rd = extract_bits(ins, 11, 7);
        uint32_t imm_raw = 0;
        imm_raw |= (extract_bits(ins, 19, 12) << 12);
        imm_raw |= (extract_bits(ins, 20, 20) << 11);
        imm_raw |= (extract_bits(ins, 30, 21) << 1);
        imm_raw |= (extract_bits(ins, 31, 31) << 20);
        int32_t imm = sign_extend(imm_raw, 21);
        return DecodedInstrution{InstrId::JAL, rd, rs1, rs2, imm};
        break;
    }
    case 0b1110011: {
        switch (extract_bits(ins, 31, 7)) {
        case 0b0000000000000000000000000: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            int32_t imm = 0;
            return DecodedInstrution{InstrId::ECALL, rd, rs1, rs2, imm};
            break;
        }
        case 0b0000000000010000000000000: {
            uint32_t rd = 0;
            uint32_t rs1 = 0;
            uint32_t rs2 = 0;
            int32_t imm = 0;
            return DecodedInstrution{InstrId::EBREAK, rd, rs1, rs2, imm};
            break;
        }
        default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
        }
        break;
    }
    default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Decode error");
    }
    return std::nullopt;
}
