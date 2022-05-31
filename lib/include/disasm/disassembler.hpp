#pragma once

#include <disasm/architecture.hpp>
#include <disasm/instruction.hpp>
#include <span>

namespace dc::disasm {

    namespace {

        template<std::derived_from<dc::hlp::TypeArrayBase> T, size_t Index>
        std::pair<size_t, std::string> disassemble(const auto &bytes) {
            using Instr = typename T::template Get<Index>;

            if (Instr::Pattern::matches(bytes))
                return { Instr::Pattern::getByteCount(), Instr::disassemble(bytes) };
            else if constexpr (Index < (T::Size - 1))
                return disassemble<T, Index + 1>(bytes);
            else
                return { 0, "" };
        }

    }



    template<dc::disasm::ArchitectureType T>
    auto disassemble(const auto &bytes) {
        std::vector<std::string> disassembly;
        size_t offset = 0x00;

        while (offset < bytes.size()) {
            auto begin = bytes.begin() + offset;

            auto [size, disas] = disassemble<typename T::Instructions, 0>(std::span { begin, bytes.end() });
            if (size < T::InstructionSizeMin) {

                for (u32 i = 0; i < T::InstructionSizeMin; i++) {
                    disassembly.push_back(fmt::format(".byte 0x{:02X}", *(begin + i)));
                    offset += 1;
                }

            }
            else {
                disassembly.push_back(disas);
                offset += size;
            }
        }

        return disassembly;
    }

}