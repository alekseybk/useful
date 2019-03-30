#pragma once
#include <cstdint>
#include <type_traits>
#include <utility>

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

        namespace detail
        {
            template<auto B, auto E, auto... Ns>
            struct increasing_sequence_helper
            {
                using type = typename increasing_sequence_helper<B + 1, E, Ns..., B>::type;
            };

            template<auto E, auto... Ns>
            struct increasing_sequence_helper<E, E, Ns...>
            {
                using type = sequence<Ns...>;
            };

            template<auto B, auto E, auto... Ns>
            struct decreasing_sequence_helper
            {
                using type = typename decreasing_sequence_helper<B - 1, E, Ns..., B>::type;
            };

            template<auto E, auto... Ns>
            struct decreasing_sequence_helper<E, E, Ns...>
            {
                using type = sequence<Ns..., E>;
            };
        }

        template<auto B, auto E>
        struct increasing_sequence
        {
            static_assert(B <= E);
            using type = typename detail::increasing_sequence_helper<B, E>::type;
        };

        template<auto B, auto E>
        struct decreasing_sequence
        {
            static_assert(B >= E);
            using type = typename detail::decreasing_sequence_helper<B, E>::type;
        };

        template<auto B, auto E>
        using make_increasing_sequence = typename increasing_sequence<B, E>::type;

        template<auto B, auto E>
        using make_decreasing_sequence = typename decreasing_sequence<B, E>::type;

        template<auto N>
        using make_sequence = make_increasing_sequence<u64(0), u64(N)>;
    }
    // namespace base
}
// namespace uf
