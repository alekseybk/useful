/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "std_import.hpp"

namespace uf
{
    namespace meta
    {
        template<typename Tp>
        inline constexpr bool is_char_type_v;

        namespace detail
        {
            template<class Tp>
            struct is_string_type_helper : public conditional_t<is_pointer_v<Tp>,
                                                      conditional_t<is_char_type_v<decay_t<remove_pointer_t<Tp>>>,
                                                          true_type,
                                                          false_type>,
                                                      false_type> { };

            template<typename... Ts>
            struct is_string_type_helper<std::basic_string<Ts...>> : public true_type { };

            template<typename... Ts>
            struct is_string_type_helper<std::basic_string_view<Ts...>> : public true_type { };

            template<typename Tuple, typename N, typename... Types>
            struct tuple_remove_last_helper
            {
                using type = typename tuple_remove_last_helper<Tuple, integral_constant<u64, N::value - 1>, tuple_element_t<N::value - 1, Tuple>, Types...>::type;
            };

            template<typename Tuple, typename... Types>
            struct tuple_remove_last_helper<Tuple, integral_constant<u64, 0>, Types...>
            {
                using type = tuple<Types...>;
            };

            template<typename Tuple, typename N, typename... Ts>
            struct tuple_remove_first_helper
            {
                using type = typename tuple_remove_first_helper<Tuple, integral_constant<u64, N::value + 1>, Ts..., tuple_element_t<N::value, Tuple>>::type;
            };

            template<typename Tuple, typename... Ts>
            struct tuple_remove_first_helper<Tuple, integral_constant<u64, tuple_size_v<Tuple>>, Ts...>
            {
                using type = tuple<Ts...>;
            };

            template<typename TupleA, typename IndexA, typename TupleB, typename IndexB, typename... Types>
            struct tuple_concat_two_helper
            {
                using type = typename tuple_concat_two_helper<TupleA, integral_constant<u64, IndexA::value + 1>,
                                                              TupleB, integral_constant<u64, IndexB::value>,
                                                              Types..., tuple_element_t<IndexA::value, TupleA>>::type;
            };

            template<typename TupleA, typename TupleB, typename IndexB, typename... Types>
            struct tuple_concat_two_helper<TupleA, integral_constant<u64, tuple_size_v<TupleA>>, TupleB, IndexB, Types...>
            {
                using type = typename tuple_concat_two_helper<TupleA, integral_constant<u64, tuple_size_v<TupleA>>,
                                                              TupleB, integral_constant<u64, IndexB::value + 1>,
                                                              Types..., tuple_element_t<IndexB::value, TupleB>>::type;
            };

            template<typename TupleA, typename TupleB, typename... Types>
            struct tuple_concat_two_helper<TupleA, integral_constant<u64, tuple_size_v<TupleA>>, TupleB,
                                           integral_constant<u64, tuple_size_v<TupleB>>, Types...>
            {
                using type = tuple<Types...>;
            };

            template<typename TupleA, typename TupleB>
            struct tuple_concat_two
            {
                using type = typename tuple_concat_two_helper<TupleA, integral_constant<u64, 0>, TupleB, integral_constant<u64, 0>>::type;
            };

            template<typename Result, typename Tuple, typename... Tuples>
            struct tuple_concat_helper
            {
                using type = typename tuple_concat_helper<typename tuple_concat_two<Result, Tuple>::type, Tuples...>::type;
            };

            template<typename Result, typename Tuple>
            struct tuple_concat_helper<Result, Tuple>
            {
                using type = typename tuple_concat_two<Result, Tuple>::type;
            };

            template<typename Tuple, u64 N, typename... Types>
            struct tuple_push_back_helper
            {
                using type = typename tuple_push_back_helper<Tuple, N - 1, tuple_element_t<N - 1, Tuple>, Types...>::type;
            };

            template<typename Tuple, typename... Types>
            struct tuple_push_back_helper<Tuple, 0, Types...>
            {
                using type = tuple<Types...>;
            };

            template<typename Tuple, typename N, typename... Types>
            struct tuple_push_front_helper
            {
                using type = typename tuple_push_front_helper<Tuple, integral_constant<u64, N::value + 1>, Types..., tuple_element_t<N::value, Tuple>>::type;
            };

            template<typename Tuple, typename... Types>
            struct tuple_push_front_helper<Tuple, integral_constant<u64, tuple_size_v<Tuple>>, Types...>
            {
                using type = tuple<Types...>;
            };

            template<typename Tp, u64 Index, typename Tuple, typename CurIndex, typename... Ts>
            struct tuple_replace_index_helper
            {
                using type = typename tuple_replace_index_helper<Tp, Index, Tuple,
                             integral_constant<u64, CurIndex::value + 1>, Ts..., tuple_element_t<CurIndex::value, Tuple>>::type;
            };

            template<typename Tp, u64 Index, typename Tuple, typename... Ts>
            struct tuple_replace_index_helper<Tp, Index, Tuple, integral_constant<u64, Index>, Ts...>
            {
                using type = typename tuple_replace_index_helper<Tp, Index, Tuple, integral_constant<u64, Index + 1>, Ts..., Tp>::type;
            };

            template<typename Tp, u64 Index, typename Tuple, typename... Ts>
            struct tuple_replace_index_helper<Tp, Index, Tuple, integral_constant<u64, tuple_size_v<Tuple>>, Ts...>
            {
                using type = tuple<Ts...>;
            };

            template<typename Tp, u64 N, typename... Types>
            struct tuple_same_n_helper
            {
                using type = typename tuple_same_n_helper<Tp, N - 1, Tp, Types...>::type;
            };

            template<typename Tp, typename... Types>
            struct tuple_same_n_helper<Tp, 0, Types...>
            {
                using type = tuple<Types...>;
            };

            template<template<typename...> typename Clean, typename Tuple>
            struct tuple_apply_to_clean_helper;

            template<template<typename...> typename Clean, typename... Ts>
            struct tuple_apply_to_clean_helper<Clean, tuple<Ts...>>
            {
                using type = Clean<Ts...>;
            };

            template<template<typename...> typename Expected, typename Tested>
            struct is_same_tmpl_helper : public false_type { };

            template<template<typename...> typename Expected, typename... Ts>
            struct is_same_tmpl_helper<Expected, Expected<Ts...>> : public true_type { };

            template<u64 Begin, u64 End, u64... Ns>
            struct make_custom_index_sequence_helper
            {
                using type = typename make_custom_index_sequence_helper<Begin + 1, End, Ns..., Begin>::type;
            };

            template<u64 End, u64... Ns>
            struct make_custom_index_sequence_helper<End, End, Ns...>
            {
                using type = index_sequence<Ns...>;
            };
        }
        // end namespace detail

        template<typename Tp, typename... Ts>
        inline constexpr bool is_one_of_the_v = (is_same_v<Tp, Ts> || ...);

        template<typename Tp, typename... Ts>
        inline constexpr bool is_all_of_the_v = (is_same_v<Tp, Ts> && ...);

        template<template<typename> typename P, typename... Ts>
        inline constexpr bool is_one_of_the_p_v = (P<Ts>::value || ...);

        template<template<typename> typename P, typename... Ts>
        inline constexpr bool is_all_of_the_p_v = (P<Ts>::value && ...);

        template<typename Tp>
        struct is_tuple : public false_type { };

        template<typename... Ts>
        struct is_tuple<tuple<Ts...>> : public true_type { };

        template<typename Tp>
        inline constexpr bool is_tuple_v = is_tuple<Tp>::value;

        template<typename Tp>
        struct is_pair : public false_type { };

        template<typename F, typename S>
        struct is_pair<pair<F, S>> : public true_type { };

        template<typename Tp>
        inline constexpr bool is_pair_v = is_pair<Tp>::value;

        template<typename Tp>
        inline constexpr bool is_char_type_v = is_one_of_the_v<Tp, char, wchar_t, char16_t, char32_t>;

        template<class Tp>
        struct is_string_type
        {
            static constexpr bool value = detail::is_string_type_helper<decay_t<Tp>>::value;
        };

        template<class Tp>
        inline constexpr bool is_string_type_v = is_string_type<Tp>::value;

        template<typename Tuple, typename = enable_if_t<(tuple_size_v<Tuple> >= 1)>>
        struct tuple_remove_last
        {
            using type = typename detail::tuple_remove_last_helper<Tuple, integral_constant<u64, tuple_size_v<Tuple> - 1>>::type;
        };

        template<typename Tuple>
        using tuple_remove_last_t = typename tuple_remove_last<Tuple>::type;

        template<typename Tuple, u64 N, typename = enable_if_t<(tuple_size_v<Tuple> >= N)>>
        struct tuple_remove_last_n
        {
            using type = typename detail::tuple_remove_last_helper<Tuple, integral_constant<u64, tuple_size_v<Tuple> - N>>::type;
        };

        template<typename Tuple, u64 N>
        using tuple_remove_last_n_t = typename tuple_remove_last_n<Tuple, N>::type;

        template<typename Tuple, typename = enable_if_t<(tuple_size_v<Tuple> >= 1)>>
        struct tuple_remove_first
        {
            using type = typename detail::tuple_remove_first_helper<Tuple, integral_constant<u64, 1>>::type;
        };

        template<typename Tuple>
        using tuple_remove_first_t = typename tuple_remove_first<Tuple>::type;

        template<typename Tuple, u64 N, typename = enable_if_t<(tuple_size_v<Tuple> >= N)>>
        struct tuple_remove_first_n
        {
            using type = typename detail::tuple_remove_first_helper<Tuple, integral_constant<u64, N>>::type;
        };

        template<typename Tuple, u64 N>
        using tuple_remove_first_n_t = typename tuple_remove_first_n<Tuple, N>::type;

        template<typename Tuple, typename... Types>
        struct tuple_push_back
        {
            using type = typename detail::tuple_push_back_helper<Tuple, tuple_size_v<Tuple>, Types...>::type;
        };

        template<typename Tuple, typename... Types>
        using tuple_push_back_t = typename tuple_push_back<Tuple, Types...>::type;

        template<typename Tuple, typename... Types>
        struct tuple_push_front
        {
            using type = typename detail::tuple_push_front_helper<Tuple, integral_constant<u64, 0>, Types...>::type;
        };

        template<typename Tuple, typename... Types>
        using tuple_push_front_t = typename tuple_push_front<Tuple, Types...>::type;

        template<typename... Tuples>
        struct tuple_concat
        {
            using type = typename detail::tuple_concat_helper<tuple<>, Tuples...>::type;
        };

        template<typename... Tuples>
        using tuple_concat_t = typename tuple_concat<Tuples...>::type;

        template<typename Tp, u64 N>
        struct tuple_same_n
        {
            using type = typename detail::tuple_same_n_helper<Tp, N>::type;
        };

        template<typename Tp, u64 N>
        using tuple_same_n_t = typename tuple_same_n<Tp, N>::type;

        template<typename Tp, u64 Index, typename Tuple>
        struct tuple_replace_index
        {
            using type = typename detail::tuple_replace_index_helper<Tp, Index, Tuple, integral_constant<u64, 0>>::type;
        };

        template<typename Tp, u64 Index, typename Tuple>
        using tuple_replace_index_t = typename tuple_replace_index<Tp, Index, Tuple>::type;

        template<template<typename...> typename Clean, typename Tuple>
        struct tuple_apply_to_clean
        {
            using type = typename detail::tuple_apply_to_clean_helper<Clean, Tuple>::type;
        };

        template<template<typename...> typename Clean, typename Tuple>
        using tuple_apply_to_clean_t = typename tuple_apply_to_clean<Clean, Tuple>::type;

        template<typename Tp>
        struct clean;

        template<template<typename...> typename Tp, typename... Ts>
        struct clean<Tp<Ts...>>
        {
            template<typename... Types>
            using type = Tp<Types...>;

            using args_tuple = tuple<Ts...>;
        };

        template<template<typename...> typename Expected, typename Tested>
        struct is_same_tmpl
        {
            static constexpr bool value = detail::is_same_tmpl_helper<Expected, decay_t<Tested>>::value;
        };

        template<template<typename...> typename Expected, typename Tested>
        inline constexpr bool is_same_tmpl_v =  is_same_tmpl<Expected, decay_t<Tested>>::value;

        template<typename... Ts>
        using first_t = tuple_element_t<0, tuple<Ts...>>;

        template<typename... Ts>
        using last_t = tuple_element_t<sizeof...(Ts) - 1, tuple<Ts...>>;

        template<typename... Ts>
        struct is_same_all
        {
            using type = first_t<Ts...>;

            static constexpr bool value = (is_same_v<Ts, type> && ...);
        };

        template<typename... Ts>
        inline constexpr bool is_same_all_v = is_same_all<Ts...>::value;

        template<class Function>
        struct function_info;

        template<class R, class... Args>
        struct function_info<function<R(Args...)>>
        {
            static constexpr u64 arg_n = sizeof...(Args);

            using res_t = R;

            template<u64 N>
            using arg_t = tuple_element_t<N, tuple<Args...>>;
        };

        template<u64 Begin, u64 End>
        using make_custom_index_sequence = typename detail::make_custom_index_sequence_helper<Begin, End>::type;

        template<bool X, typename Tp = void>
        using disable_if_t = enable_if_t<!X, Tp>;

        template<class Tp, typename = void>
        struct has_begin_end : public false_type { };

        template<class Tp>
        struct has_begin_end<Tp, void_t<decltype(declval<Tp>().begin()), decltype(declval<Tp>().end())>> : public true_type { };

        template<class Tp>
        inline constexpr bool has_begin_end_v = has_begin_end<Tp>::value;
    }
}
