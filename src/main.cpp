#include <cerrno>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

#include <gflags/gflags.h>

#include "CPU.hpp"

DEFINE_string(i, "stdin", "Input file path");
DEFINE_bool(debug, false, "Enable debug print");
DEFINE_uint64(max_ticks, 1e9, "Limit of simulation ticks");

int main(int argc, char **argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (FLAGS_i == "stdin") {
        std::cerr << "ELF input is required (use -i <file>)" << std::endl;
        return -EINVAL;
    }

    CPU cpu(std::cerr, FLAGS_debug);

    std::vector<std::string> program_args;
    if (argc > 1) {
        program_args.emplace_back(FLAGS_i);
        for (int i = 1; i < argc; ++i) {
            program_args.emplace_back(argv[i]);
        }
    }

    cpu.loadElf(FLAGS_i);
    if (!program_args.empty()) {
        cpu.initArgs(program_args);
    }

    try {
        uint64_t ticks = FLAGS_max_ticks;
        uint64_t remain_ticks = cpu.simulate(ticks);

        // cpu.dumpRegs();

        if (remain_ticks == -1) {
            std::cerr << std::format("Exceeded ticks limit {}", FLAGS_max_ticks);
            return -EOVERFLOW;
        }

        std::cerr << "Executed in " << (ticks - remain_ticks) << " ticks";

    } catch (const std::system_error &ex) {
        cpu.dumpRegs();
        std::cerr << "System error: code=" << ex.code().value() << " ("
                  << ex.code().message() << ") reason='" << ex.what() << "'\n";
        return -ex.code().value();
    } catch (const std::exception &ex) {
        cpu.dumpRegs();
        std::cerr << "Exception, reason='" << ex.what() << "'\n";
        return -EFAULT;
    }

    return 0;
}
