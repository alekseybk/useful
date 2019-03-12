/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include <cstdint>
#include <type_traits>

namespace uf
{
    inline namespace ints
    {
        using u8 = std::uint8_t;
        using i8 = std::int8_t;

        using u16 = std::uint16_t;
        using i16 = std::int16_t;

        using u32 = std::uint32_t;
        using i32 = std::int32_t;

        using u64 = std::uint64_t;
        using i64 = std::int64_t;
    }
    // namespace ints

    template<u64 N>
    using uconst = std::integral_constant<u64, N>;

    template<i64 N>
    using iconst = std::integral_constant<i64, N>;

    enum class sfinae;
    inline constexpr sfinae sdef{0};

    template<typename... Ts>
    using sfinae_t = sfinae;

    template<bool X>
    using enif = std::enable_if_t<X, sfinae>;

    template<bool X>
    using disif = std::enable_if_t<!X, sfinae>;

    template<typename Tp>
    struct type_identity
    {
        using type = Tp;
    };

    template<typename Tp>
    using type_identity_t = typename type_identity<Tp>::type;
}
// namespace uf
