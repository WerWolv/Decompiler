#pragma once

#include <dc.hpp>
#include <helpers/bit_pattern.hpp>
#include <helpers/type_array.hpp>

namespace dc::disasm {

    template<typename T>
    concept Instruction = requires(std::vector<u8> &&data) {
        typename T::Pattern;
        T::Mnemonic;
        { T::disassemble(data) };
    };

    template<Instruction ... Ts>
    struct InstructionArray : public hlp::TypeArray<Ts...> {};

}