#pragma once

#include <dc.hpp>
#include <ast/ast_node.hpp>
#include <helpers/bit_pattern.hpp>
#include <helpers/type_array.hpp>

#include <vector>
#include <fmt/format.h>
#include <fmt/compile.h>

namespace dc::disasm {

    template<typename T>
    concept InstructionType = requires(u64 address, std::vector<u8> &&data) {
        typename T::Pattern;
        T::Mnemonic;
        { T::disassemble(address, data) } -> std::same_as<std::string>;
        { T::decompile(address, data) } -> std::same_as<std::vector<std::unique_ptr<ast::ASTNode>>>;
        requires (sizeof(T) == sizeof(hlp::Empty));
    };

    template<InstructionType ... Ts>
    struct InstructionArray : public hlp::TypeArray<Ts...> {};

    enum class Category {
        FunctionCall,
        FunctionReturn,
        ConditionalJump,
        UnconditionalJump,
        Arithmetic,
        MemoryAccess,
        RegisterAccess,
        Other
    };

    template<hlp::StaticString MnemonicValue, hlp::StaticString PatternValue, Category InstructionCategory, std::endian Endian>
    struct Instruction {
    public:
        constexpr static auto Mnemonic = MnemonicValue.data;
        constexpr static auto Category = InstructionCategory;
        using Pattern = hlp::BitPattern<PatternValue, Endian>;

        template<char C>
        struct Placeholder {
            static constexpr auto get(const auto &bytes) {
                return Pattern::template getPlaceholderValue<C>(bytes);
            }
        };
    };

}