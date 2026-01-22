require "yaml"

require_relative "../Generic/base"
require_relative "../Generic/scope"
require_relative "../Generic/var"
require_relative "../Target/RISC-V/regfile"

module SimInfra
    class InterpreterGenerator
        def initialize(ir_path, out_path)
            @ir_path = ir_path
            @out_path = out_path
        end

        def run
            instructions = YAML.unsafe_load_file(@ir_path)
            code = generate_cpp(instructions)
            File.write(@out_path, code)
        end

        def generate_cpp(instructions)
            lines = []
            lines << "#pragma once"
            lines << "#include <cstdint>"
            lines << "#include <system_error>"
            lines << "#include \"decoder.hpp\""
            lines << ""
            lines << "struct CPU;"
            lines << ""
            lines << "inline int32_t as_signed(uint32_t v) noexcept {"
            lines << "    return static_cast<int32_t>(v);"
            lines << "}"
            lines << ""
            lines << "inline bool execute(CPU& cpu, const DecodedInstrution& ins) {"
            lines << "    switch (ins.id) {"
            instructions.each do |ins|
                lines << "    case InstrId::#{ins[:name]}: {"
                lines.concat(generate_instruction_cpp(ins, 2))
                lines << "        break;"
                lines << "    }"
            end
            lines << "    default:"
            lines << "        throw std::system_error(std::make_error_code(std::errc::invalid_argument), \"Unknown instruction\");"
            lines << "    }"
            lines << "    return false;"
            lines << "}"
            lines.join("\n") + "\n"
        end

        def generate_instruction_cpp(ins, indent_level)
            indent = "    " * indent_level
            lines = []
            arg_names = ins[:args].map(&:name)
            scope = ins[:code]

            scope.tree.each do |stmt|
                lines.concat(generate_stmt(stmt, arg_names, indent_level))
            end
            lines
        end

        def generate_stmt(stmt, arg_names, indent_level)
            indent = "    " * indent_level
            lines = []
            name = stmt.name
            ops = stmt.oprnds

            case name
            when :new_var
                var = ops[0]
                var_name = var.name.to_s
                if arg_names.include?(var.name)
                    if var.name == :imm
                        lines << "#{indent}int32_t #{var_name} = ins.imm;"
                    else
                        lines << "#{indent}uint32_t #{var_name} = ins.#{var_name};"
                    end
                elsif var.name == :pc
                    lines << "#{indent}uint32_t #{var_name} = cpu.pc;"
                else
                    lines << "#{indent}uint32_t #{var_name} = 0;"
                end
            when :new_const
            when :read
                dst = var_name(ops[0])
                idx = operand_expr(ops[2])
                lines << "#{indent}#{dst} = cpu.readReg(#{idx});"
            when :write
                idx = operand_expr(ops[1])
                val = operand_expr(ops[2])
                lines << "#{indent}cpu.writeReg(#{idx}, #{val});"
            when :add
                lines << "#{indent}#{var_name(ops[0])} = #{operand_expr(ops[1])} + #{operand_expr(ops[2])};"
            when :sub
                lines << "#{indent}#{var_name(ops[0])} = #{operand_expr(ops[1])} - #{operand_expr(ops[2])};"
            when :and
                lines << "#{indent}#{var_name(ops[0])} = #{operand_expr(ops[1])} & #{operand_expr(ops[2])};"
            when :or
                lines << "#{indent}#{var_name(ops[0])} = #{operand_expr(ops[1])} | #{operand_expr(ops[2])};"
            when :xor
                lines << "#{indent}#{var_name(ops[0])} = #{operand_expr(ops[1])} ^ #{operand_expr(ops[2])};"
            when :shl
                lines << "#{indent}#{var_name(ops[0])} = #{operand_expr(ops[1])} << (#{operand_expr(ops[2])} & 0x1f);"
            when :shr
                lines << "#{indent}#{var_name(ops[0])} = #{operand_expr(ops[1])} >> (#{operand_expr(ops[2])} & 0x1f);"
            when :sar
                lines << "#{indent}#{var_name(ops[0])} = static_cast<uint32_t>(as_signed(#{operand_expr(ops[1])}) >> (#{operand_expr(ops[2])} & 0x1f));"
            when :eq
                lines << "#{indent}#{var_name(ops[0])} = (#{operand_expr(ops[1])} == #{operand_expr(ops[2])}) ? 1u : 0u;"
            when :ne
                lines << "#{indent}#{var_name(ops[0])} = (#{operand_expr(ops[1])} != #{operand_expr(ops[2])}) ? 1u : 0u;"
            when :lt
                lines << "#{indent}#{var_name(ops[0])} = (as_signed(#{operand_expr(ops[1])}) < as_signed(#{operand_expr(ops[2])})) ? 1u : 0u;"
            when :ge
                lines << "#{indent}#{var_name(ops[0])} = (as_signed(#{operand_expr(ops[1])}) >= as_signed(#{operand_expr(ops[2])})) ? 1u : 0u;"
            when :ltu
                lines << "#{indent}#{var_name(ops[0])} = (#{operand_expr(ops[1])} < #{operand_expr(ops[2])}) ? 1u : 0u;"
            when :geu
                lines << "#{indent}#{var_name(ops[0])} = (#{operand_expr(ops[1])} >= #{operand_expr(ops[2])}) ? 1u : 0u;"
            when :select
                lines << "#{indent}#{var_name(ops[0])} = (#{operand_expr(ops[1])} != 0) ? #{operand_expr(ops[2])} : #{operand_expr(ops[3])};"
            when :branch
                lines << "#{indent}cpu.branch(#{operand_expr(ops[0])});"
            when :mem_read
                dst = var_name(ops[0])
                addr = operand_expr(ops[1])
                size = mem_size_bits(ops[2])
                sign = mem_sign(ops[3])
                lines << "#{indent}#{dst} = cpu.memRead(#{addr}, #{size}, #{sign});"
            when :mem_write
                addr = operand_expr(ops[0])
                val = operand_expr(ops[1])
                size = mem_size_bits(ops[2])
                lines << "#{indent}cpu.memWrite(#{addr}, #{size}, #{val});"
            when :syscall
                lines << "#{indent}uint32_t num = cpu.readReg(17);"
                lines << "#{indent}uint32_t a0 = cpu.readReg(10);"
                lines << "#{indent}uint32_t a1 = cpu.readReg(11);"
                lines << "#{indent}uint32_t a2 = cpu.readReg(12);"
                lines << "#{indent}if (cpu.syscall(num, a0, a1, a2)) return true;"
            when :ebreak
                lines << "#{indent}cpu.ebreak();"
            when :fence
                lines << "#{indent}cpu.fence();"
            else
                raise "Unsupported IR stmt: #{name}"
            end
            lines
        end

        def var_name(op)
            return op.name.to_s if op.is_a?(SimInfra::Var)
            op.to_s
        end

        def operand_expr(op)
            return op.value.to_s if op.is_a?(SimInfra::Constant)
            return op.to_s if op.is_a?(Integer)
            return op.name.to_s if op.is_a?(SimInfra::Var)
            return op.to_s if op.is_a?(Symbol)
            op.to_s
        end

        def mem_size_bits(size_sym)
            case size_sym
            when :b8 then 8
            when :b16 then 16
            when :b32 then 32
            else
                raise "Unknown mem size #{size_sym}"
            end
        end

        def mem_sign(sign_sym)
            case sign_sym
            when :s8, :s16, :s32 then "true"
            when :u8, :u16, :u32 then "false"
            else
                raise "Unknown mem sign #{sign_sym}"
            end
        end
    end
end

ir_path = "./IR.yaml"
out_path = "./generated/interpreter.hpp"
SimInfra::InterpreterGenerator.new(ir_path, out_path).run
