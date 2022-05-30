#pragma once

#include <dc.hpp>

namespace dc::hlp {

    [[nodiscard]]
    constexpr bool isLower(char c) {
        return c >= 'a' && c <= 'z';
    }

    [[nodiscard]]
    constexpr bool isUpper(char c) {
        return c >= 'A' && c <= 'Z';
    }

}