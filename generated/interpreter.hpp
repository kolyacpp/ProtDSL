#pragma once
#include <cstdint>
#include <system_error>
#include "decoder.hpp"

struct CPU;

inline int32_t as_signed(uint32_t v) noexcept {
    return static_cast<int32_t>(v);
}

inline bool execute(CPU& cpu, const DecodedInstrution& ins) {
    switch (ins.id) {
    case InstrId::LUI: {
        uint32_t rd = ins.rd;
        int32_t imm = ins.imm;
        uint32_t _tmp1 = 0;
        _tmp1 = imm << (12 & 0x1f);
        cpu.writeReg(rd, _tmp1);
        break;
    }
    case InstrId::AUIPC: {
        uint32_t rd = ins.rd;
        uint32_t pc = cpu.pc;
        int32_t imm = ins.imm;
        uint32_t _tmp3 = 0;
        _tmp3 = imm << (12 & 0x1f);
        uint32_t _tmp4 = 0;
        _tmp4 = pc + _tmp3;
        cpu.writeReg(rd, _tmp4);
        break;
    }
    case InstrId::JAL: {
        uint32_t rd = ins.rd;
        uint32_t pc = cpu.pc;
        uint32_t _tmp6 = 0;
        _tmp6 = pc + 4;
        cpu.writeReg(rd, _tmp6);
        int32_t imm = ins.imm;
        uint32_t _tmp7 = 0;
        _tmp7 = pc + imm;
        cpu.branch(_tmp7);
        break;
    }
    case InstrId::JALR: {
        uint32_t pc = cpu.pc;
        uint32_t _tmp9 = 0;
        _tmp9 = pc + 4;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp10 = 0;
        _tmp10 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp11 = 0;
        _tmp11 = _tmp10 + imm;
        uint32_t _tmp13 = 0;
        _tmp13 = _tmp11 & -2;
        uint32_t rd = ins.rd;
        cpu.writeReg(rd, _tmp9);
        cpu.branch(_tmp13);
        break;
    }
    case InstrId::ADDI: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp14 = 0;
        _tmp14 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp15 = 0;
        _tmp15 = _tmp14 + imm;
        cpu.writeReg(rd, _tmp15);
        break;
    }
    case InstrId::SLTI: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp16 = 0;
        _tmp16 = cpu.readReg(rs1);
        uint32_t rd = ins.rd;
        int32_t imm = ins.imm;
        uint32_t _tmp17 = 0;
        _tmp17 = (as_signed(_tmp16) < as_signed(imm)) ? 1u : 0u;
        uint32_t _tmp20 = 0;
        _tmp20 = (_tmp17 != 0) ? 1 : 0;
        cpu.writeReg(rd, _tmp20);
        break;
    }
    case InstrId::SLTIU: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp21 = 0;
        _tmp21 = cpu.readReg(rs1);
        uint32_t rd = ins.rd;
        int32_t imm = ins.imm;
        uint32_t _tmp22 = 0;
        _tmp22 = (_tmp21 < imm) ? 1u : 0u;
        uint32_t _tmp25 = 0;
        _tmp25 = (_tmp22 != 0) ? 1 : 0;
        cpu.writeReg(rd, _tmp25);
        break;
    }
    case InstrId::XORI: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp26 = 0;
        _tmp26 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp27 = 0;
        _tmp27 = _tmp26 ^ imm;
        cpu.writeReg(rd, _tmp27);
        break;
    }
    case InstrId::ORI: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp28 = 0;
        _tmp28 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp29 = 0;
        _tmp29 = _tmp28 | imm;
        cpu.writeReg(rd, _tmp29);
        break;
    }
    case InstrId::ANDI: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp30 = 0;
        _tmp30 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp31 = 0;
        _tmp31 = _tmp30 & imm;
        cpu.writeReg(rd, _tmp31);
        break;
    }
    case InstrId::SLLI: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp32 = 0;
        _tmp32 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp33 = 0;
        _tmp33 = _tmp32 << (imm & 0x1f);
        cpu.writeReg(rd, _tmp33);
        break;
    }
    case InstrId::SRLI: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp34 = 0;
        _tmp34 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp35 = 0;
        _tmp35 = _tmp34 >> (imm & 0x1f);
        cpu.writeReg(rd, _tmp35);
        break;
    }
    case InstrId::SRAI: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp36 = 0;
        _tmp36 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp37 = 0;
        _tmp37 = static_cast<uint32_t>(as_signed(_tmp36) >> (imm & 0x1f));
        cpu.writeReg(rd, _tmp37);
        break;
    }
    case InstrId::ADD: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp38 = 0;
        _tmp38 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp39 = 0;
        _tmp39 = cpu.readReg(rs2);
        uint32_t _tmp40 = 0;
        _tmp40 = _tmp38 + _tmp39;
        cpu.writeReg(rd, _tmp40);
        break;
    }
    case InstrId::SUB: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp41 = 0;
        _tmp41 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp42 = 0;
        _tmp42 = cpu.readReg(rs2);
        uint32_t _tmp43 = 0;
        _tmp43 = _tmp41 - _tmp42;
        cpu.writeReg(rd, _tmp43);
        break;
    }
    case InstrId::SLL: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp44 = 0;
        _tmp44 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp45 = 0;
        _tmp45 = cpu.readReg(rs2);
        uint32_t _tmp46 = 0;
        _tmp46 = _tmp44 << (_tmp45 & 0x1f);
        cpu.writeReg(rd, _tmp46);
        break;
    }
    case InstrId::SLT: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp47 = 0;
        _tmp47 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp48 = 0;
        _tmp48 = cpu.readReg(rs2);
        uint32_t rd = ins.rd;
        uint32_t _tmp49 = 0;
        _tmp49 = (as_signed(_tmp47) < as_signed(_tmp48)) ? 1u : 0u;
        uint32_t _tmp52 = 0;
        _tmp52 = (_tmp49 != 0) ? 1 : 0;
        cpu.writeReg(rd, _tmp52);
        break;
    }
    case InstrId::SLTU: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp53 = 0;
        _tmp53 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp54 = 0;
        _tmp54 = cpu.readReg(rs2);
        uint32_t rd = ins.rd;
        uint32_t _tmp55 = 0;
        _tmp55 = (_tmp53 < _tmp54) ? 1u : 0u;
        uint32_t _tmp58 = 0;
        _tmp58 = (_tmp55 != 0) ? 1 : 0;
        cpu.writeReg(rd, _tmp58);
        break;
    }
    case InstrId::XOR: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp59 = 0;
        _tmp59 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp60 = 0;
        _tmp60 = cpu.readReg(rs2);
        uint32_t _tmp61 = 0;
        _tmp61 = _tmp59 ^ _tmp60;
        cpu.writeReg(rd, _tmp61);
        break;
    }
    case InstrId::SRL: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp62 = 0;
        _tmp62 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp63 = 0;
        _tmp63 = cpu.readReg(rs2);
        uint32_t _tmp64 = 0;
        _tmp64 = _tmp62 >> (_tmp63 & 0x1f);
        cpu.writeReg(rd, _tmp64);
        break;
    }
    case InstrId::SRA: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp65 = 0;
        _tmp65 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp66 = 0;
        _tmp66 = cpu.readReg(rs2);
        uint32_t _tmp67 = 0;
        _tmp67 = static_cast<uint32_t>(as_signed(_tmp65) >> (_tmp66 & 0x1f));
        cpu.writeReg(rd, _tmp67);
        break;
    }
    case InstrId::OR: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp68 = 0;
        _tmp68 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp69 = 0;
        _tmp69 = cpu.readReg(rs2);
        uint32_t _tmp70 = 0;
        _tmp70 = _tmp68 | _tmp69;
        cpu.writeReg(rd, _tmp70);
        break;
    }
    case InstrId::AND: {
        uint32_t rd = ins.rd;
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp71 = 0;
        _tmp71 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp72 = 0;
        _tmp72 = cpu.readReg(rs2);
        uint32_t _tmp73 = 0;
        _tmp73 = _tmp71 & _tmp72;
        cpu.writeReg(rd, _tmp73);
        break;
    }
    case InstrId::BEQ: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp74 = 0;
        _tmp74 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp75 = 0;
        _tmp75 = cpu.readReg(rs2);
        uint32_t _tmp76 = 0;
        _tmp76 = (_tmp74 == _tmp75) ? 1u : 0u;
        uint32_t pc = cpu.pc;
        int32_t imm = ins.imm;
        uint32_t _tmp77 = 0;
        _tmp77 = pc + imm;
        uint32_t _tmp79 = 0;
        _tmp79 = pc + 4;
        uint32_t _tmp80 = 0;
        _tmp80 = (_tmp76 != 0) ? _tmp77 : _tmp79;
        cpu.branch(_tmp80);
        break;
    }
    case InstrId::BNE: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp81 = 0;
        _tmp81 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp82 = 0;
        _tmp82 = cpu.readReg(rs2);
        uint32_t _tmp83 = 0;
        _tmp83 = (_tmp81 != _tmp82) ? 1u : 0u;
        uint32_t pc = cpu.pc;
        int32_t imm = ins.imm;
        uint32_t _tmp84 = 0;
        _tmp84 = pc + imm;
        uint32_t _tmp86 = 0;
        _tmp86 = pc + 4;
        uint32_t _tmp87 = 0;
        _tmp87 = (_tmp83 != 0) ? _tmp84 : _tmp86;
        cpu.branch(_tmp87);
        break;
    }
    case InstrId::BLT: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp88 = 0;
        _tmp88 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp89 = 0;
        _tmp89 = cpu.readReg(rs2);
        uint32_t _tmp90 = 0;
        _tmp90 = (as_signed(_tmp88) < as_signed(_tmp89)) ? 1u : 0u;
        uint32_t pc = cpu.pc;
        int32_t imm = ins.imm;
        uint32_t _tmp91 = 0;
        _tmp91 = pc + imm;
        uint32_t _tmp93 = 0;
        _tmp93 = pc + 4;
        uint32_t _tmp94 = 0;
        _tmp94 = (_tmp90 != 0) ? _tmp91 : _tmp93;
        cpu.branch(_tmp94);
        break;
    }
    case InstrId::BGE: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp95 = 0;
        _tmp95 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp96 = 0;
        _tmp96 = cpu.readReg(rs2);
        uint32_t _tmp97 = 0;
        _tmp97 = (as_signed(_tmp95) >= as_signed(_tmp96)) ? 1u : 0u;
        uint32_t pc = cpu.pc;
        int32_t imm = ins.imm;
        uint32_t _tmp98 = 0;
        _tmp98 = pc + imm;
        uint32_t _tmp100 = 0;
        _tmp100 = pc + 4;
        uint32_t _tmp101 = 0;
        _tmp101 = (_tmp97 != 0) ? _tmp98 : _tmp100;
        cpu.branch(_tmp101);
        break;
    }
    case InstrId::BLTU: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp102 = 0;
        _tmp102 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp103 = 0;
        _tmp103 = cpu.readReg(rs2);
        uint32_t _tmp104 = 0;
        _tmp104 = (_tmp102 < _tmp103) ? 1u : 0u;
        uint32_t pc = cpu.pc;
        int32_t imm = ins.imm;
        uint32_t _tmp105 = 0;
        _tmp105 = pc + imm;
        uint32_t _tmp107 = 0;
        _tmp107 = pc + 4;
        uint32_t _tmp108 = 0;
        _tmp108 = (_tmp104 != 0) ? _tmp105 : _tmp107;
        cpu.branch(_tmp108);
        break;
    }
    case InstrId::BGEU: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp109 = 0;
        _tmp109 = cpu.readReg(rs1);
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp110 = 0;
        _tmp110 = cpu.readReg(rs2);
        uint32_t _tmp111 = 0;
        _tmp111 = (_tmp109 >= _tmp110) ? 1u : 0u;
        uint32_t pc = cpu.pc;
        int32_t imm = ins.imm;
        uint32_t _tmp112 = 0;
        _tmp112 = pc + imm;
        uint32_t _tmp114 = 0;
        _tmp114 = pc + 4;
        uint32_t _tmp115 = 0;
        _tmp115 = (_tmp111 != 0) ? _tmp112 : _tmp114;
        cpu.branch(_tmp115);
        break;
    }
    case InstrId::LB: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp116 = 0;
        _tmp116 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp117 = 0;
        _tmp117 = _tmp116 + imm;
        uint32_t rd = ins.rd;
        uint32_t _tmp118 = 0;
        _tmp118 = cpu.memRead(_tmp117, 8, true);
        cpu.writeReg(rd, _tmp118);
        break;
    }
    case InstrId::LH: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp119 = 0;
        _tmp119 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp120 = 0;
        _tmp120 = _tmp119 + imm;
        uint32_t rd = ins.rd;
        uint32_t _tmp121 = 0;
        _tmp121 = cpu.memRead(_tmp120, 16, true);
        cpu.writeReg(rd, _tmp121);
        break;
    }
    case InstrId::LW: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp122 = 0;
        _tmp122 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp123 = 0;
        _tmp123 = _tmp122 + imm;
        uint32_t rd = ins.rd;
        uint32_t _tmp124 = 0;
        _tmp124 = cpu.memRead(_tmp123, 32, true);
        cpu.writeReg(rd, _tmp124);
        break;
    }
    case InstrId::LBU: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp125 = 0;
        _tmp125 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp126 = 0;
        _tmp126 = _tmp125 + imm;
        uint32_t rd = ins.rd;
        uint32_t _tmp127 = 0;
        _tmp127 = cpu.memRead(_tmp126, 8, false);
        cpu.writeReg(rd, _tmp127);
        break;
    }
    case InstrId::LHU: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp128 = 0;
        _tmp128 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp129 = 0;
        _tmp129 = _tmp128 + imm;
        uint32_t rd = ins.rd;
        uint32_t _tmp130 = 0;
        _tmp130 = cpu.memRead(_tmp129, 16, false);
        cpu.writeReg(rd, _tmp130);
        break;
    }
    case InstrId::SB: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp131 = 0;
        _tmp131 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp132 = 0;
        _tmp132 = _tmp131 + imm;
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp133 = 0;
        _tmp133 = cpu.readReg(rs2);
        cpu.memWrite(_tmp132, 8, _tmp133);
        break;
    }
    case InstrId::SH: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp134 = 0;
        _tmp134 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp135 = 0;
        _tmp135 = _tmp134 + imm;
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp136 = 0;
        _tmp136 = cpu.readReg(rs2);
        cpu.memWrite(_tmp135, 16, _tmp136);
        break;
    }
    case InstrId::SW: {
        uint32_t rs1 = ins.rs1;
        uint32_t _tmp137 = 0;
        _tmp137 = cpu.readReg(rs1);
        int32_t imm = ins.imm;
        uint32_t _tmp138 = 0;
        _tmp138 = _tmp137 + imm;
        uint32_t rs2 = ins.rs2;
        uint32_t _tmp139 = 0;
        _tmp139 = cpu.readReg(rs2);
        cpu.memWrite(_tmp138, 32, _tmp139);
        break;
    }
    case InstrId::ECALL: {
        uint32_t num = cpu.readReg(17);
        uint32_t a0 = cpu.readReg(10);
        uint32_t a1 = cpu.readReg(11);
        uint32_t a2 = cpu.readReg(12);
        if (cpu.syscall(num, a0, a1, a2)) return true;
        break;
    }
    case InstrId::EBREAK: {
        cpu.ebreak();
        break;
    }
    case InstrId::FENCE: {
        cpu.fence();
        break;
    }
    case InstrId::FENCE_I: {
        cpu.fence();
        break;
    }
    default:
        throw std::system_error(std::make_error_code(std::errc::invalid_argument), "Unknown instruction");
    }
    return false;
}
