#pragma once
#include <cstdint>
#include <type_traits>
#include <utility>
#include <functional>

namespace uf
{
    inline namespace base
    {      
        inline namespace sized
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
        // inline namespace sized

        enum class sfinae;
        inline constexpr sfinae SF{0};

        namespace detail
        {
            template<typename Any, typename...>
            struct sfinae_t_impl
            {
                using type = std::conditional_t<std::is_same_v<Any, void>, sfinae, sfinae>;
            };
        }

        template<typename... Ts>
        using sfinae_t = typename detail::sfinae_t_impl<void, Ts...>::type;

        template<bool X>
        using enif = std::enable_if_t<X, sfinae>;

        template<bool X>
        using disif = std::enable_if_t<!X, sfinae>;

        template<typename Tp>
        struct type_identity
        {
            using type = Tp;
        };

        template<auto N>
        using constant = std::integral_constant<decltype(N), N>;

        template<auto... Ns>
        struct sequence
        {
            static constexpr u64 size = sizeof...(Ns);

            template<auto N>
            static constexpr auto value = std::get<N>(std::tuple(Ns...));

            template<auto N>
            using type = std::tuple_element_t<N, std::tuple<decltype(Ns)...>>;

            template<typename Tp>
            using cast = sequence<static_cast<Tp>(Ns)...>;

            template<typename Tp>
            using cast_std = std::integer_sequence<Tp, Ns...>;
        };
    }
    // inline namespace base
}
// namespace uf
