#pragma once

#include <dc.hpp>

#include <algorithm>

namespace dc::hlp {

    template<size_t N>
    class StaticString {
    public:
        constexpr StaticString(const char (&string)[N]) {
            std::copy_n(string, size(), this->data);
        }

        [[nodiscard]]
        constexpr static inline size_t size() { return N - 1; }

        [[nodiscard]]
        constexpr inline const char *begin() const {
            return this->data;
        }

        [[nodiscard]]
        constexpr inline const char *end() const {
            return this->data + size();
        }

        char data[size()];
    };

}