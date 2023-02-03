#pragma once

#include <dc.hpp>

#include <string>

namespace dc::hlp {

    [[nodiscard]]
    constexpr bool isLower(char c) {
        return c >= 'a' && c <= 'z';
    }

    [[nodiscard]]
    constexpr bool isUpper(char c) {
        return c >= 'A' && c <= 'Z';
    }

    struct Empty { };

    inline std::string trim(const std::string &string) {
        auto first = string.find_first_not_of(' ');
        if (first == std::string::npos)
            return "";

        auto last = string.find_last_not_of(' ');
        return string.substr(first, (last - first + 1));
    }

}