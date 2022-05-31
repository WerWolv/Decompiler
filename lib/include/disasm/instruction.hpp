#pragma once

#include <dc.hpp>
#include <ast/ast_node.hpp>
#include <helpers/bit_pattern.hpp>
#include <helpers/type_array.hpp>

#include <fmt/format.h>

namespace dc::disasm {

    template<typename T>
    concept InstructionType = requires(std::vector<u8> &&data) {
        typename T::Pattern;
        T::Mnemonic;
        { T::disassemble(data) } -> std::same_as<std::string>;
        requires (sizeof(T) == sizeof(hlp::Empty));
    };

    template<InstructionType ... Ts>
    struct InstructionArray : public hlp::TypeArray<Ts...> {};

    template<hlp::StaticString MnemonicValue, hlp::StaticString PatternValue, std::endian Endian>
    struct Instruction {
    public:
        constexpr static auto Mnemonic = MnemonicValue.data;
        using Pattern = hlp::BitPattern<PatternValue, Endian>;

        template<char C>
        struct Placeholder {
            static constexpr auto get(const auto &bytes) {
                return Pattern::template getPlaceholderValue<C>(bytes);
            }
        };
    };

}