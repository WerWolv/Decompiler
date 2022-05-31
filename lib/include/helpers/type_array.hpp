#pragma once

#include <tuple>

namespace dc::hlp {

    struct TypeArrayBase {};

    template<typename ... Ts>
    struct TypeArray : public TypeArrayBase {
        using Types = std::tuple<Ts...>;

        template<size_t Index>
        using Get = std::tuple_element_t<Index, Types>;

        constexpr static size_t Size = sizeof...(Ts);
    };

}