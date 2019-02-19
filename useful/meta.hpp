/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "import.hpp"

namespace uf::mt
{
    template<template<typename> typename P, typename... Ts>
    inline constexpr bool is_satisfies_one_v = (P<Ts>::value || ...);

    template<template<typename> typename P, typename... Ts>
    inline constexpr bool is_satisfies_all_v = (P<Ts>::value && ...);

    template<template<typename, typename> typename P, typename F, typename... Ts>
    inline constexpr bool is_satisfies_one_bin_f_v = (P<F, Ts>::value || ...);

    template<template<typename, typename> typename P, typename F, typename... Ts>
    inline constexpr bool is_satisfies_all_bin_f_v = (P<F, Ts>::value && ...);

    template<template<typename, typename> typename P, typename S, typename... Ts>
    inline constexpr bool is_satisfies_one_bin_s_v = (P<Ts, S>::value || ...);

    template<template<typename, typename> typename P, typename S, typename... Ts>
    inline constexpr bool is_satisfies_all_bin_s_v = (P<Ts, S>::value && ...);

    template<typename Tp, typename... Ts>
    inline constexpr bool is_one_of_v = is_satisfies_one_bin_f_v<std::is_same, Tp, Ts...>;

    template<typename Tp>
    inline constexpr bool is_char_type_v = is_one_of_v<Tp, char, wchar_t, char16_t, char32_t>;

    namespace detail
    {
        template<class Tp>
        struct is_string_type_helper : public std::conditional_t<std::is_pointer_v<Tp>,
                                                  std::conditional_t<is_char_type_v<std::remove_cv_t<std::remove_pointer_t<Tp>>>,
                                                      std::true_type,
                                                      std::false_type>,
                                                  std::false_type> { };

        template<typename... Ts>
        struct is_string_type_helper<std::basic_string<Ts...>> : public std::true_type { };

        template<typename... Ts>
        struct is_string_type_helper<std::basic_string_view<Ts...>> : public std::true_type { };

        template<typename Tuple, typename N, typename... Types>
        struct tuple_remove_last_helper
        {
            using type = typename tuple_remove_last_helper<Tuple, std::integral_constant<u64, N::value - 1>, std::tuple_element_t<N::value - 1, Tuple>, Types...>::type;
        };

        template<typename Tuple, typename... Types>
        struct tuple_remove_last_helper<Tuple, std::integral_constant<u64, 0>, Types...>
        {
            using type = std::tuple<Types...>;
        };

        template<typename Tuple, typename N, typename... Ts>
        struct tuple_remove_first_helper
        {
            using type = typename tuple_remove_first_helper<Tuple, std::integral_constant<u64, N::value + 1>, Ts..., std::tuple_element_t<N::value, Tuple>>::type;
        };

        template<typename Tuple, typename... Ts>
        struct tuple_remove_first_helper<Tuple, std::integral_constant<u64, std::tuple_size_v<Tuple>>, Ts...>
        {
            using type = std::tuple<Ts...>;
        };

        template<typename TupleA, typename IndexA, typename TupleB, typename IndexB, typename... Types>
        struct tuple_concat_two_helper
        {
            using type = typename tuple_concat_two_helper<TupleA, std::integral_constant<u64, IndexA::value + 1>,
                                                          TupleB, std::integral_constant<u64, IndexB::value>,
                                                          Types..., std::tuple_element_t<IndexA::value, TupleA>>::type;
        };

        template<typename TupleA, typename TupleB, typename IndexB, typename... Types>
        struct tuple_concat_two_helper<TupleA, std::integral_constant<u64, std::tuple_size_v<TupleA>>, TupleB, IndexB, Types...>
        {
            using type = typename tuple_concat_two_helper<TupleA, std::integral_constant<u64, std::tuple_size_v<TupleA>>,
                                                          TupleB, std::integral_constant<u64, IndexB::value + 1>,
                                                          Types..., std::tuple_element_t<IndexB::value, TupleB>>::type;
        };

        template<typename TupleA, typename TupleB, typename... Types>
        struct tuple_concat_two_helper<TupleA, std::integral_constant<u64, std::tuple_size_v<TupleA>>, TupleB, std::integral_constant<u64, std::tuple_size_v<TupleB>>, Types...>
        {
            using type = std::tuple<Types...>;
        };

        template<typename TupleA, typename TupleB>
        struct tuple_concat_two
        {
            using type = typename tuple_concat_two_helper<TupleA, std::integral_constant<u64, 0>, TupleB, std::integral_constant<u64, 0>>::type;
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
            using type = typename tuple_push_back_helper<Tuple, N - 1, std::tuple_element_t<N - 1, Tuple>, Types...>::type;
        };

        template<typename Tuple, typename... Types>
        struct tuple_push_back_helper<Tuple, 0, Types...>
        {
            using type = std::tuple<Types...>;
        };

        template<typename Tuple, typename N, typename... Types>
        struct tuple_push_front_helper
        {
            using type = typename tuple_push_front_helper<Tuple, std::integral_constant<u64, N::value + 1>, Types..., std::tuple_element_t<N::value, Tuple>>::type;
        };

        template<typename Tuple, typename... Types>
        struct tuple_push_front_helper<Tuple, std::integral_constant<u64, std::tuple_size_v<Tuple>>, Types...>
        {
            using type = std::tuple<Types...>;
        };

        template<typename Tp, u64 Index, typename Tuple, typename CurIndex, typename... Ts>
        struct tuple_replace_index_helper
        {
            using type = typename tuple_replace_index_helper<Tp, Index, Tuple,
                         std::integral_constant<u64, CurIndex::value + 1>, Ts..., std::tuple_element_t<CurIndex::value, Tuple>>::type;
        };

        template<typename Tp, u64 Index, typename Tuple, typename... Ts>
        struct tuple_replace_index_helper<Tp, Index, Tuple, std::integral_constant<u64, Index>, Ts...>
        {
            using type = typename tuple_replace_index_helper<Tp, Index, Tuple, std::integral_constant<u64, Index + 1>, Ts..., Tp>::type;
        };

        template<typename Tp, u64 Index, typename Tuple, typename... Ts>
        struct tuple_replace_index_helper<Tp, Index, Tuple, std::integral_constant<u64, std::tuple_size_v<Tuple>>, Ts...>
        {
            using type = std::tuple<Ts...>;
        };

        template<typename Tp, u64 N, typename... Types>
        struct tuple_same_n_helper
        {
            using type = typename tuple_same_n_helper<Tp, N - 1, Tp, Types...>::type;
        };

        template<typename Tp, typename... Types>
        struct tuple_same_n_helper<Tp, 0, Types...>
        {
            using type = std::tuple<Types...>;
        };

        template<template<typename...> typename Expected, typename Tested>
        struct is_same_template_helper : public std::false_type { };

        template<template<typename...> typename Expected, typename... Ts>
        struct is_same_template_helper<Expected, Expected<Ts...>> : public std::true_type { };

        template<u64 Begin, u64 End, u64... Ns>
        struct make_custom_index_sequence_helper
        {
            using type = typename make_custom_index_sequence_helper<Begin + 1, End, Ns..., Begin>::type;
        };

        template<u64 End, u64... Ns>
        struct make_custom_index_sequence_helper<End, End, Ns...>
        {
            using type = std::index_sequence<Ns...>;
        };
    }
    // namespace detail

    template<typename Tp>
    struct is_tuple : public std::false_type { };

    template<typename... Ts>
    struct is_tuple<std::tuple<Ts...>> : public std::true_type { };

    template<typename Tp>
    inline constexpr bool is_tuple_v = is_tuple<Tp>::value;

    template<typename Tp>
    struct is_pair : public std::false_type { };

    template<typename F, typename S>
    struct is_pair<std::pair<F, S>> : public std::true_type { };

    template<typename Tp>
    inline constexpr bool is_pair_v = is_pair<Tp>::value;

    template<class Tp>
    struct is_string_type
    {
        static constexpr bool value = detail::is_string_type_helper<std::decay_t<Tp>>::value;
    };

    template<class Tp>
    inline constexpr bool is_string_type_v = is_string_type<Tp>::value;

    template<typename Tuple, typename = std::enable_if_t<(std::tuple_size_v<Tuple> >= 1)>>
    struct tuple_remove_last
    {
        using type = typename detail::tuple_remove_last_helper<Tuple, std::integral_constant<u64, std::tuple_size_v<Tuple> - 1>>::type;
    };

    template<typename Tuple>
    using tuple_remove_last_t = typename tuple_remove_last<Tuple>::type;

    template<typename Tuple, u64 N, typename = std::enable_if_t<(std::tuple_size_v<Tuple> >= N)>>
    struct tuple_remove_last_n
    {
        using type = typename detail::tuple_remove_last_helper<Tuple, std::integral_constant<u64, std::tuple_size_v<Tuple> - N>>::type;
    };

    template<typename Tuple, u64 N>
    using tuple_remove_last_n_t = typename tuple_remove_last_n<Tuple, N>::type;

    template<typename Tuple, typename = std::enable_if_t<(std::tuple_size_v<Tuple> >= 1)>>
    struct tuple_remove_first
    {
        using type = typename detail::tuple_remove_first_helper<Tuple, std::integral_constant<u64, 1>>::type;
    };

    template<typename Tuple>
    using tuple_remove_first_t = typename tuple_remove_first<Tuple>::type;

    template<typename Tuple, u64 N, typename = std::enable_if_t<(std::tuple_size_v<Tuple> >= N)>>
    struct tuple_remove_first_n
    {
        using type = typename detail::tuple_remove_first_helper<Tuple, std::integral_constant<u64, N>>::type;
    };

    template<typename Tuple, u64 N>
    using tuple_remove_first_n_t = typename tuple_remove_first_n<Tuple, N>::type;

    template<typename Tuple, typename... Types>
    struct tuple_push_back
    {
        using type = typename detail::tuple_push_back_helper<Tuple, std::tuple_size_v<Tuple>, Types...>::type;
    };

    template<typename Tuple, typename... Types>
    using tuple_push_back_t = typename tuple_push_back<Tuple, Types...>::type;

    template<typename Tuple, typename... Types>
    struct tuple_push_front
    {
        using type = typename detail::tuple_push_front_helper<Tuple, std::integral_constant<u64, 0>, Types...>::type;
    };

    template<typename Tuple, typename... Types>
    using tuple_push_front_t = typename tuple_push_front<Tuple, Types...>::type;

    template<typename... Tuples>
    struct tuple_concat
    {
        using type = typename detail::tuple_concat_helper<std::tuple<>, Tuples...>::type;
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
        using type = typename detail::tuple_replace_index_helper<Tp, Index, Tuple, std::integral_constant<u64, 0>>::type;
    };

    template<typename Tp, u64 Index, typename Tuple>
    using tuple_replace_index_t = typename tuple_replace_index<Tp, Index, Tuple>::type;

    template<typename Tp>
    struct clean;

    template<template<typename...> typename Tp, typename... Ts>
    struct clean<Tp<Ts...>>
    {
        static constexpr u64 args = sizeof...(Ts);

        template<typename... Ts_>
        using type = Tp<Ts_...>;

        template<u64 N>
        using nth = std::tuple_element_t<N, std::tuple<Ts...>>;
    };

    template<typename Tp, typename... Ts>
    using clean_t = typename clean<Tp>::template type<Ts...>;

    template<template<typename...> typename Clean, typename Tp>
    struct clean_copy_args;

    template<template<typename...> typename Clean, template<typename...> typename Tp, typename... Ts>
    struct clean_copy_args<Clean, Tp<Ts...>>
    {
        using type = Clean<Ts...>;
    };

    template<template<typename...> typename Clean, typename Tp>
    using clean_copy_args_t = typename clean_copy_args<Clean, Tp>::type;

    template<template<typename...> typename Expected, typename Tested>
    struct is_same_template
    {
        static constexpr bool value = detail::is_same_template_helper<Expected, std::decay_t<Tested>>::value;
    };

    template<template<typename...> typename Expected, typename Tested>
    inline constexpr bool is_same_template_v =  is_same_template<Expected, std::decay_t<Tested>>::value;

    template<typename... Ts>
    using first_t = std::tuple_element_t<0, std::tuple<Ts...>>;

    template<typename... Ts>
    using last_t = std::tuple_element_t<sizeof...(Ts) - 1, std::tuple<Ts...>>;

    template<class F>
    struct function_trait;

    template<class R, class... Args>
    struct function_trait<R(Args...)>
    {
        static constexpr u64 arity = sizeof...(Args);

        using result = R;

        template<u64 N>
        using arg = std::tuple_element_t<N, std::tuple<Args...>>;
    };

    template<class R, class... Args>
    struct function_trait<std::function<R(Args...)>> : public function_trait<R(Args...)> { };

    template<class R, class... Args>
    struct function_trait<R(*)(Args...)> : public function_trait<R(Args...)> { };

    template<class C, class R, class... Args>
    struct function_trait<R(C::*)(Args...)> : public function_trait<R(Args...)> { };

    template<u64 Begin, u64 End>
    using make_custom_index_sequence = typename detail::make_custom_index_sequence_helper<Begin, End>::type;

    template<bool X, typename Tp = void>
    struct disable_if;

    template<typename Tp>
    struct disable_if<false, Tp>
    {
        using type = Tp;
    };

    template<bool X, typename Tp = void>
    using disable_if_t = typename disable_if<X, Tp>::type;

    template<class Tp, typename = void>
    struct is_iterable : public std::false_type { };

    template<class Tp>
    struct is_iterable<Tp, std::void_t<decltype(std::declval<Tp>().begin()), decltype(std::declval<Tp>().end())>> : public std::true_type { };

    template<class Tp>
    inline constexpr bool is_iterable_v = is_iterable<Tp>::value;

    template<typename Tp, typename = void>
    struct is_dereferenceable : public std::false_type { };

    template<typename Tp>
    struct is_dereferenceable<Tp, std::void_t<decltype(*std::declval<Tp>())>> : public std::true_type { };

    template<class Tp>
    inline constexpr bool is_dereferenceable_v = is_dereferenceable<Tp>::value;

    template<typename... Ts>
    struct is_same_all
    {
        static constexpr bool value = is_satisfies_all_bin_f_v<std::is_same, first_t<Ts...>, Ts...>;
    };

    template<>
    struct is_same_all<> : public std::true_type { };

    template<typename... Ts>
    inline constexpr bool is_same_all_v = is_same_all<Ts...>::value;
}
// namespace uf::mt
