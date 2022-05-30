#pragma once

#include <tuple>
#include <disasm/instruction.hpp>

namespace dc::disasm::arm {

    struct InstrTest1 {
        using Pattern = hlp::BitPattern<"0101'1010'aaaa'aaaa">;
        constexpr static auto Mnemonic = "test1";

        static std::string disassemble(const auto &bytes) {
            const auto value = Pattern::getPlaceholderValue<'a'>(bytes);

            return std::string(Mnemonic) + " " + std::to_string(value);
        }
    };

    struct InstrTest2 {
        using Pattern = hlp::BitPattern<"0101'1011'aaaa'aaaa">;
        constexpr static auto Mnemonic = "test2";

        static std::string disassemble(const auto &bytes) {
            const auto value = Pattern::getPlaceholderValue<'a'>(bytes);

            return std::string(Mnemonic) + " " + std::to_string(value);
        }
    };

    using Instructions = InstructionArray<
        InstrTest1,
        InstrTest2
    >;

}