#pragma once

#include <disasm/instruction.hpp>
#include <span>

namespace dc::disasm {

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

    template<std::derived_from<dc::hlp::TypeArrayBase> T>
    auto disassemble(const auto &bytes) {
        std::vector<std::string> disassembly;
        size_t offset = 0x00;

        while (offset < bytes.size()) {
            auto [size, disas] = disassemble<T, 0>(std::span { bytes.begin() + offset, bytes.end() });

            disassembly.push_back(disas);

            offset += size;
            if (size == 0)
                break;
        }

        return disassembly;
    }

}