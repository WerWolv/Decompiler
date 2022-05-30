#pragma once

namespace dc::hlp {

    struct TypeArrayBase {};

    template<typename ... Ts>
    struct TypeArray : public TypeArrayBase {
        constexpr static std::tuple<Ts...> Types = { };

        template<size_t Index>
        using Get = std::remove_cvref_t<decltype(std::get<Index>(Types))>;

        constexpr static size_t Size = sizeof...(Ts);
    };

}