#pragma once

#include <concepts>

#include <helpers/utils.hpp>
#include <helpers/type_array.hpp>

namespace dc::disasm {

    template<typename T>
    concept ArchitectureType = requires {
        typename T::Instructions;
        T::InstructionSizeMin;
        requires (sizeof(T) == sizeof(hlp::Empty));
    };

}