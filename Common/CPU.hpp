#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

#include <elfio/elfio.hpp>

#include "../generated/decoder.hpp"

struct CPU;
bool execute(CPU &cpu, const DecodedInstrution &ins);

struct CPU {
    CPU(std::ostream &debug_out = std::cerr, bool dump_ticks = false,
        uint32_t stack_size = 1u << 20)
        : stack_size(stack_size), debug_out(debug_out), dump_ticks(dump_ticks) {
    }

    uint64_t simulate(uint64_t ticks) {
        while (ticks > 0) {
            if (dump_ticks) {
                dumpRegs();
            }
            bool stop = step();
            --ticks;
            if (stop) {
                return ticks;
            }
        }
        return -1;
    }

    bool step() {
        uint32_t w = nextInstruction();
        auto decoded = decode(w);
        if (!decoded) {
            throw std::system_error(
                std::make_error_code(std::errc::invalid_argument),
                "Decode failed");
        }

        next_pc = pc + 4;
        bool stop = execute(*this, *decoded);
        pc = next_pc;
        return stop;
    }

    uint32_t readReg(uint32_t idx) const { return idx == 0 ? 0 : X.at(idx); }

    void writeReg(uint32_t idx, uint32_t value) {
        if (idx != 0) {
            X.at(idx) = value;
        }
    }

    void branch(uint32_t target) { next_pc = target; }

    void loadElf(const std::string &path) {
        ELFIO::elfio reader;
        if (!reader.load(path)) {
            throw std::system_error(
                std::make_error_code(std::errc::invalid_argument),
                "Failed to load ELF file");
        }

        for (const auto &seg : reader.segments) {
            if (seg->get_type() != ELFIO::PT_LOAD)
                continue;

            auto addr = seg->get_virtual_address();
            auto memsz = seg->get_memory_size();
            auto filesz = seg->get_file_size();

            if (addr + memsz > mem.size()) {
                mem.resize(addr + memsz, 0);
            }

            std::memcpy(mem.data() + addr, seg->get_data(), filesz);
        }

        mem.resize(mem.size() + stack_size, 0);

        pc = reader.get_entry();
    }

    void initArgs(const std::vector<std::string> &args) {
        uint32_t sp = mem.size();
        std::vector<uint32_t> argv_ptrs;
        argv_ptrs.reserve(args.size());

        for (auto it = args.rbegin(); it != args.rend(); ++it) {
            const std::string &s = *it;
            uint32_t len = s.size() + 1;
            sp -= len;
            std::memcpy(mem.data() + sp, s.data(), len);
            argv_ptrs.push_back(sp);
        }

        std::reverse(argv_ptrs.begin(), argv_ptrs.end());
        sp -= sp % 4;

        uint32_t argc = args.size();
        uint32_t argv_addr = sp - (argc + 1) * 4;
        uint32_t argc_addr = argv_addr - 4;

        memWrite(argc_addr, 32, argc);
        for (uint32_t i = 0; i < argc; ++i) {
            memWrite(argv_addr + i * 4, 32, argv_ptrs[i]);
        }
        memWrite(argv_addr + argc * 4, 32, 0);

        writeReg(2, argc_addr);
        writeReg(10, argc);
        writeReg(11, argv_addr);
    }

    uint32_t memRead(uint32_t addr, uint32_t size_bits, bool sign) {
        uint32_t size_bytes = size_bits / 8;
        if (size_bytes == 0)
            return 0;
        if (addr + size_bytes > mem.size()) {
            throw std::system_error(
                std::make_error_code(std::errc::result_out_of_range),
                "Memory read out of range");
        }

        uint32_t value = 0;
        std::memcpy(&value, mem.data() + addr, size_bytes);

        if (!sign)
            return value;

        switch (size_bits) {
        case 8:
            return static_cast<uint32_t>(static_cast<int8_t>(value));
        case 16:
            return static_cast<uint32_t>(static_cast<int16_t>(value));
        case 32:
            return value;
        default:
            throw std::system_error(
                std::make_error_code(std::errc::invalid_argument),
                "Unsupported sign extend size");
        }
    }

    void memWrite(uint32_t addr, uint32_t size_bits, uint32_t value) {
        uint32_t size_bytes = size_bits / 8;
        if (size_bytes == 0)
            return;
        if (addr + size_bytes > mem.size()) {
            throw std::system_error(
                std::make_error_code(std::errc::result_out_of_range),
                "Memory write out of range");
        }

        std::memcpy(mem.data() + addr, &value, size_bytes);
    }

    bool syscall(uint32_t num, uint32_t a0, uint32_t a1, uint32_t a2) {
        auto read_string = [&](uint32_t addr) {
            const auto begin = mem.begin() + addr;
            const auto end = std::find(begin, mem.end(), 0);
            if (end == mem.end()) {
                throw std::system_error(
                    std::make_error_code(std::errc::invalid_argument),
                    "bad string");
            }
            return std::string(begin, end);
        };

        switch (num) {
        case 93: // exit
            if (a0 != 0) {
                throw std::system_error(std::error_code(-static_cast<int>(a0), std::generic_category()), "Exit");
            }
            return true;
        case 64: { // write(fd, buf, len)
            ssize_t cnt = ::write(a0, mem.data() + a1, a2);
            writeReg(10, cnt);
            return false;
        }
        case 63: { // read(fd, buf, len)
            ssize_t cnt = ::read(a0, mem.data() + a1, a2);
            writeReg(10, cnt);
            return false;
        }
        case 1024: { // open(path, flags, mode)
            std::string path = read_string(a0);
            int fd = ::open(path.c_str(), a1, a2);
            writeReg(10, fd);
            return false;
        }
        case 57: { // close(fd)
            int rc = ::close(a0);
            writeReg(10, rc);
            return false;
        }
        default:
            throw std::system_error(
                std::make_error_code(std::errc::function_not_supported),
                std::format("Unknown syscall {}", num));
        }
    }

    void ebreak() {
        throw std::system_error(
            std::make_error_code(std::errc::operation_canceled), "EBREAK");
    }

    void fence() {}

    uint32_t nextInstruction() { return fetchFromMem(pc); }

    uint32_t fetchFromMem(uint32_t addr) {
        if (addr + 4 > mem.size()) {
            throw std::system_error(
                std::make_error_code(std::errc::result_out_of_range),
                "Instruction fetch out of range");
        }
        if (addr % 4) {
            throw std::system_error(
                std::make_error_code(std::errc::invalid_argument),
                "Unaligned instruction fetch");
        }

        uint32_t value = 0;
        std::memcpy(&value, mem.data() + addr, sizeof(value));
        return value;
    }

    void dumpRegs() {
        debug_out << std::format("pc = {}\n", pc);
        for (std::size_t i = 0; i < X.size(); ++i) {
            if (i % 8 == 0)
                debug_out << '\t';
            debug_out << std::format("x{:02}={}", i, X[i]);
            debug_out << ((i + 1) % 8 == 0 || i + 1 == X.size() ? "\n" : "  ");
        }
        debug_out << '\n';
    }

    std::array<uint32_t, 32> X{};
    std::vector<uint8_t> mem;

    size_t stack_size;

    uint32_t pc = 0;
    uint32_t next_pc = 0;
    std::ostream &debug_out;
    bool dump_ticks;
};

#include "../generated/interpreter.hpp"
