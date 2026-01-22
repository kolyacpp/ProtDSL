require "yaml"

require_relative "../Generic/base"
require_relative "../Generic/scope"
require_relative "../Generic/var"
require_relative "../Target/RISC-V/regfile"

module SimInfra
    class DecoderGenerator
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
            info_list = instructions.map { |ins| build_info(ins) }
            decode_tree = make_head(info_list)
            lines = []
            lines << "#pragma once"
            lines << "#include <cstdint>"
            lines << "#include <optional>"
            lines << "#include <system_error>"
            lines << ""
            lines << "enum class InstrId : uint32_t {"
            instructions.each do |ins|
                lines << "    #{ins[:name]},"
            end
            lines << "    INVALID"
            lines << "};"
            lines << ""
            lines << "struct DecodedInstrution {"
            lines << "    InstrId id;"
            lines << "    uint32_t rd;"
            lines << "    uint32_t rs1;"
            lines << "    uint32_t rs2;"
            lines << "    int32_t imm;"
            lines << "};"
            lines << ""
            lines << "constexpr uint32_t extract_bits(uint32_t w, uint32_t hi, uint32_t lo) noexcept {"
            lines << "    uint32_t width = hi - lo + 1;"
            lines << "    if (width >= 32) return w;"
            lines << "    return (w >> lo) & ((uint32_t(1) << width) - 1);"
            lines << "}"
            lines << ""
            lines << "constexpr int32_t sign_extend(uint32_t x, uint32_t bits) noexcept {"
            lines << "    if (bits == 0) return 0;"
            lines << "    uint32_t m = uint32_t(1) << (bits - 1);"
            lines << "    return int32_t((x ^ m) - m);"
            lines << "}"
            lines << ""
            lines << "inline std::optional<DecodedInstrution> decode(uint32_t ins) {"
            lines.concat(generate_tree_cpp(decode_tree, 1))
            lines << "    return std::nullopt;"
            lines << "}"
            lines.join("\n") + "\n"
        end

        def generate_tree_cpp(node, indent_level)
            indent = "    " * indent_level
            lines = []
            if node[:leaf]
                raise "Ambiguous decode leaf for #{node[:leaf].map { |i| i[:name] }.join(", ")}" if node[:leaf].length != 1
                lines.concat(generate_return_cpp(node[:leaf].first, indent_level))
                return lines
            end

            msb, lsb = node[:range]
            lines << "#{indent}switch (extract_bits(ins, #{msb}, #{lsb})) {"
            node[:nodes].keys.sort.each do |case_val|
                child = node[:nodes][case_val]
                lines << "#{indent}case #{format_case_value(case_val, msb - lsb + 1)}: {"
                lines.concat(generate_tree_cpp(child, indent_level + 1))
                lines << "#{indent}    break;"
                lines << "#{indent}}"
            end
            lines << "#{indent}default: throw std::system_error(std::make_error_code(std::errc::invalid_argument), \"Decode error\");"
            lines << "#{indent}}"
            lines
        end

        def format_case_value(value, width)
            binary = value.to_s(2).rjust(width, "0")
            "0b#{binary}"
        end

        def generate_return_cpp(ins, indent_level)
            indent = "    " * indent_level
            lines = []
            lines << "#{indent}uint32_t rd = 0;"
            lines << "#{indent}uint32_t rs1 = 0;"
            lines << "#{indent}uint32_t rs2 = 0;"

            if ins[:rd]
                lines << "#{indent}rd = extract_bits(ins, #{ins[:rd][:hi]}, #{ins[:rd][:lo]});"
            end
            if ins[:rs1]
                lines << "#{indent}rs1 = extract_bits(ins, #{ins[:rs1][:hi]}, #{ins[:rs1][:lo]});"
            end
            if ins[:rs2]
                lines << "#{indent}rs2 = extract_bits(ins, #{ins[:rs2][:hi]}, #{ins[:rs2][:lo]});"
            end

            if ins[:imm_bits] > 0
                lines << "#{indent}uint32_t imm_raw = 0;"
                ins[:imm_parts].each do |part|
                    lines << "#{indent}imm_raw |= (extract_bits(ins, #{part[:from]}, #{part[:to]}) << #{part[:lo]});"
                end
                lines << "#{indent}int32_t imm = sign_extend(imm_raw, #{ins[:imm_bits]});"
            else
                lines << "#{indent}int32_t imm = 0;"
            end

            lines << "#{indent}return DecodedInstrution{InstrId::#{ins[:name]}, rd, rs1, rs2, imm};"
            lines
        end

        def build_info(ins)
            mask, value = build_match_mask(ins[:fields])
            imm_parts = collect_imm_parts(ins[:fields])
            imm_bits = imm_parts.empty? ? 0 : imm_parts.map { |p| p[:hi] }.max + 1
            {
                name: ins[:name],
                fields: ins[:fields],
                mask: mask,
                value: value,
                rd: find_reg_range(ins[:fields], :rd),
                rs1: find_reg_range(ins[:fields], :rs1),
                rs2: find_reg_range(ins[:fields], :rs2),
                imm_parts: imm_parts,
                imm_bits: imm_bits
            }
        end

        def make_head(instructions)
            lead_bits = get_lead_bits(instructions, 0)
            return { leaf: instructions } if lead_bits.empty?

            msb, lsb = get_maj_range(lead_bits)
            width = msb - lsb + 1
            mask = ((1 << width) - 1) << lsb
            tree = { range: [msb, lsb], nodes: {} }

            (0..((1 << width) - 1)).each do |node_value|
                node = node_value << lsb
                child = make_child(node, mask, instructions)
                tree[:nodes][node_value] = child if child
            end

            return { leaf: instructions } if tree[:nodes].empty?
            tree
        end

        def make_child(node, separ_mask, instructions)
            sublist = instructions.select { |ins| ((ins[:value] ^ node) & separ_mask) == 0 }
            return nil if sublist.empty?
            return { leaf: sublist } if sublist.length == 1

            lead_bits = get_lead_bits(sublist, separ_mask)
            return { leaf: sublist } if lead_bits.empty?

            msb, lsb = get_maj_range(lead_bits)
            width = msb - lsb + 1
            new_mask = separ_mask | (((1 << width) - 1) << lsb)
            subtree = { range: [msb, lsb], nodes: {} }

            (0..((1 << width) - 1)).each do |node_value|
                child = make_child(node | (node_value << lsb), new_mask, sublist)
                subtree[:nodes][node_value] = child if child
            end

            return { leaf: sublist } if subtree[:nodes].empty?
            subtree
        end

        def get_lead_bits(instructions, separ_mask)
            bits = []
            0.upto(31) do |bit|
                next if ((separ_mask >> bit) & 1) == 1
                next unless instructions.all? { |ins| ((ins[:mask] >> bit) & 1) == 1 }
                bits << bit
            end
            bits
        end

        def get_maj_range(lead_bits)
            sorted = lead_bits.sort
            best = [sorted.first, sorted.first]
            start = sorted.first
            prev = sorted.first
            sorted.drop(1).each do |bit|
                if bit == prev + 1
                    prev = bit
                    next
                end
                best = pick_best_range(best, [start, prev])
                start = bit
                prev = bit
            end
            best = pick_best_range(best, [start, prev])
            [best[1], best[0]]
        end

        def pick_best_range(best, candidate)
            best_len = best[1] - best[0] + 1
            cand_len = candidate[1] - candidate[0] + 1
            return candidate if cand_len > best_len
            return candidate if cand_len == best_len && candidate[1] > best[1]
            best
        end

        def build_match_mask(fields)
            mask = 0
            value = 0
            fields.each do |field|
                next unless field.is_a?(SimInfra::Field)
                next unless field.value.is_a?(Integer)
                width = field.from - field.to + 1
                field_mask = ((1 << width) - 1) << field.to
                mask |= field_mask
                value |= (field.value << field.to) & field_mask
            end
            [mask, value]
        end

        def find_reg_range(fields, name)
            field = fields.find { |f| f.is_a?(SimInfra::Field) && f.value == :reg && f.name == name }
            return nil unless field
            { hi: field.from, lo: field.to }
        end

        def collect_imm_parts(fields)
            parts = []
            fields.each do |field|
                if field.is_a?(SimInfra::ImmFieldPart)
                    parts << { from: field.from, to: field.to, hi: field.hi, lo: field.lo }
                elsif field.is_a?(SimInfra::Field) && field.value == :imm
                    width = field.from - field.to + 1
                    parts << { from: field.from, to: field.to, hi: width - 1, lo: 0 }
                end
            end
            parts
        end
    end
end


ir_path = "./IR.yaml"
out_path = "./generated/decoder.hpp"
SimInfra::DecoderGenerator.new(ir_path, out_path).run

