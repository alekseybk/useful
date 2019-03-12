/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "import.hpp"

namespace uf::mt
{
    template<typename, typename Tp>
    struct cloner_tt : public type_identity<Tp> { };

    template<typename, typename Tp, Tp N>
    struct cloner_tn : public std::integral_constant<Tp, N> { };

    template<u64, typename Tp>
    struct cloner_nt : public type_identity<Tp> { };

    template<u64, typename Tp, Tp N>
    struct cloner_nn : public std::integral_constant<Tp, N> { };

    template<u64, typename Tp>
    auto& clone_ref(Tp& x)
    {
        return x;
    }

    template<typename, typename Tp>
    auto& clone_ref(Tp& x)
    {
        return x;
    }

    namespace detail
    {
        template<typename T, typename N, typename... Ts>
        struct tuple_remove_last_helper
        {
            using type = typename tuple_remove_last_helper<T, uconst<N::value - 1>, std::tuple_element_t<N::value - 1, T>, Ts...>::type;
        };

        template<typename T, typename... Ts>
        struct tuple_remove_last_helper<T, uconst<0>, Ts...>
        {
            using type = std::tuple<Ts...>;
        };

        template<typename T, typename N, typename... Ts>
        struct tuple_remove_first_helper
        {
            using type = typename tuple_remove_first_helper<T, uconst<N::value + 1>, Ts..., std::tuple_element_t<N::value, T>>::type;
        };

        template<typename T, typename... Ts>
        struct tuple_remove_first_helper<T, uconst<std::tuple_size_v<T>>, Ts...>
        {
            using type = std::tuple<Ts...>;
        };

        template<typename Tp, typename S>
        struct tuple_clone_type_helper;

        template<typename Tp, u64... Ns>
        struct tuple_clone_type_helper<Tp, std::index_sequence<Ns...>>
        {
            using type = std::tuple<typename cloner_nt<Ns, Tp>::type...>;
        };

        template<typename SCur, typename S, typename Sf, u64... Ns>
        struct seq_remove_helper;

        template<u64... SCurArgs, u64 SArg, u64... SArgs, u64... Ns>
        struct seq_remove_helper<std::index_sequence<SCurArgs...>, std::index_sequence<SArg, SArgs...>, enif<((SArg == Ns) || ...)>, Ns...>
        {
            using type = typename seq_remove_helper<std::index_sequence<SCurArgs...>, std::index_sequence<SArgs...>, sfinae, Ns...>::type;
        };

        template<u64... SCurArgs, u64 SArg, u64... SArgs, u64... Ns>
        struct seq_remove_helper<std::index_sequence<SCurArgs...>, std::index_sequence<SArg, SArgs...>, enif<((SArg != Ns) && ...)>, Ns...>
        {
            using type = typename seq_remove_helper<std::index_sequence<SCurArgs..., SArg>, std::index_sequence<SArgs...>, sfinae, Ns...>::type;
        };

        template<typename SCur, u64... Ns>
        struct seq_remove_helper<SCur, std::index_sequence<>, sfinae, Ns...>
        {
            using type = SCur;
        };
    }
    // namespace detail

    template<typename S, typename... Ts>
    struct tuple_from_seq;

    template<u64... Ns, typename... Ts>
    struct tuple_from_seq<std::index_sequence<Ns...>, Ts...>
    {
        using type = std::tuple<std::tuple_element_t<Ns, std::tuple<Ts...>>...>;
    };

    template<typename S, typename... Ts>
    using tuple_from_seq_t = typename tuple_from_seq<S, Ts...>::type;

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

    template<typename Tp, typename Sf = sfinae>
    struct is_decayed : public std::false_type { };

    template<typename Tp>
    struct is_decayed<Tp, disif<std::is_const_v<Tp> || std::is_volatile_v<Tp> || std::is_reference_v<Tp>>> : public std::true_type { };

    template<typename Tp>
    inline constexpr bool is_decayed_v = is_decayed<Tp>::value;

    template<typename T, u64 N, enif<(std::tuple_size_v<T> >= N)> = sdef>
    struct tuple_remove_last_n
    {
        using type = typename detail::tuple_remove_last_helper<T, uconst<std::tuple_size_v<T> - N>>::type;
    };

    template<typename T, u64 N>
    using tuple_remove_last_n_t = typename tuple_remove_last_n<T, N>::type;

    template<typename T>
    struct tuple_remove_last : public tuple_remove_last_n<T, 1> { };

    template<typename T>
    using tuple_remove_last_t = typename tuple_remove_last<T>::type;

    template<typename T, u64 N, enif<(std::tuple_size_v<T> >= N)> = sdef>
    struct tuple_remove_first_n
    {
        using type = typename detail::tuple_remove_first_helper<T, std::integral_constant<u64, N>>::type;
    };

    template<typename T, u64 N>
    using tuple_remove_first_n_t = typename tuple_remove_first_n<T, N>::type;

    template<typename T>
    struct tuple_remove_first : public tuple_remove_first_n<T, 1> { };

    template<typename T>
    using tuple_remove_first_t = typename tuple_remove_first<T>::type;

    template<typename T, typename... Ts>
    struct tuple_push_back;

    template<typename... TArgs, typename... Ts>
    struct tuple_push_back<std::tuple<TArgs...>, Ts...>
    {
        using type = std::tuple<TArgs..., Ts...>;
    };

    template<typename T, typename... Ts>
    using tuple_push_back_t = typename tuple_push_back<T, Ts...>::type;

    template<typename T, typename... Ts>
    struct tuple_push_front;

    template<typename... TArgs, typename... Ts>
    struct tuple_push_front<std::tuple<TArgs...>, Ts...>
    {
        using type = std::tuple<Ts..., TArgs...>;
    };

    template<typename T, typename... Ts>
    using tuple_push_front_t = typename tuple_push_front<T, Ts...>::type;

    template<typename T1, typename T2>
    struct tuple_concat_two;

    template<typename... Ts1, typename... Ts2>
    struct tuple_concat_two<std::tuple<Ts1...>, std::tuple<Ts2...>>
    {
        using type = std::tuple<Ts1..., Ts2...>;
    };

    template<typename... Ts>
    using tuple_concat_two_t = typename tuple_concat_two<Ts...>::type;

    template<typename... Ts>
    struct tuple_concat;

    template<typename... Ts>
    struct tuple_concat<std::tuple<Ts...>>
    {
        using type = std::tuple<Ts...>;
    };

    template<typename T1, typename T2, typename... Ts>
    struct tuple_concat<T1, T2, Ts...>
    {
        using type = typename tuple_concat<tuple_concat_two<T1, T2>, Ts...>::type;
    };

    template<typename... Ts>
    using tuple_concat_t = typename tuple_concat<Ts...>::type;

    template<typename Tp, u64 N>
    struct tuple_clone_type
    {
        using type = typename detail::tuple_clone_type_helper<Tp, std::make_index_sequence<N>>::type;
    };

    template<typename Tp, u64 N>
    using tuple_clone_type_t = typename tuple_clone_type<Tp, N>::type;

    template<u64 B, u64 E, u64... Ns>
    struct inc_index_seq
    {
        using type = inc_index_seq<B + 1, E, Ns..., B>;
    };

    template<u64 E, u64... Ns>
    struct inc_index_seq<E, E, Ns...>
    {
        using type = std::index_sequence<Ns...>;
    };

    template<u64 B, u64 E>
    using inc_index_seq_t = typename inc_index_seq<B, E>::type;

    template<u64 B, u64 E, u64... Ns>
    struct dec_index_seq
    {
        using type = inc_index_seq<B - 1, E, Ns..., B>;
    };

    template<u64 E, u64... Ns>
    struct dec_index_seq<E, E, Ns...>
    {
        using type = std::index_sequence<Ns..., E>;
    };

    template<u64 B, u64 E>
    using dec_index_seq_t = typename dec_index_seq<B, E>::type;

    template<u64 N, typename Tp, typename T>
    struct tuple_replace_index;

    template<u64 N, typename Tp, typename... Ts>
    struct tuple_replace_index<N, Tp, std::tuple<Ts...>>
    {
        using type = tuple_concat_t<tuple_from_seq_t<inc_index_seq_t<0, N>, Ts...>, std::tuple<Tp>, tuple_from_seq_t<inc_index_seq_t<N + 1, sizeof...(Ts)>, Ts...>>;
    };

    template<u64 N, typename Tp, typename T>
    using tuple_replace_index_t = typename tuple_replace_index<N, Tp, T>::type;

    template<typename S, u64... Ns>
    struct seq_remove
    {
        using type = typename detail::seq_remove_helper<std::index_sequence<>, S, void, Ns...>::type;
    };

    template<typename S, u64... Ns>
    using seq_remove_t = typename seq_remove<S, Ns...>::type;

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
    struct clean_copy_args<Clean, Tp<Ts...>> : public type_identity<Clean<Ts...>> { };

    template<template<typename...> typename Clean, typename Tp>
    using clean_copy_args_t = typename clean_copy_args<Clean, Tp>::type;

    template<template<typename...> typename Expected, typename Tested>
    struct is_same_template : public std::false_type { };

    template<template<typename...> typename Expected, typename... Ts>
    struct is_same_template<Expected, Expected<Ts...>> : public std::true_type { };

    template<template<typename...> typename Expected, typename Tested>
    inline constexpr bool is_same_template_v =  is_same_template<Expected, std::decay_t<Tested>>::value;

    template<typename... Ts>
    struct first;

    template<typename F, typename... Ts>
    struct first<F, Ts...> : public type_identity<F> { };

    template<typename... Ts>
    using first_t = typename first<Ts...>::type;

    template<typename... Ts>
    struct last
    {
        using type = typename decltype((type_identity<Ts>{ }, ...))::type;
    };

    template<typename... Ts>
    using last_t = typename last<Ts...>::type;

    template<class F>
    struct func_info;

    template<class R, class... Args>
    struct func_info<R(Args...)>
    {
        static constexpr u64 arity = sizeof...(Args);

        using result = R;

        template<u64 N>
        using nth = std::tuple_element_t<N, std::tuple<Args...>>;
    };

    template<class R, class... Args>
    struct func_info<std::function<R(Args...)>> : public func_info<R(Args...)> { };

    template<class R, class... Args>
    struct func_info<R(*)(Args...)> : public func_info<R(Args...)> { };

    template<class C, class R, class... Args>
    struct func_info<R(C::*)(Args...)> : public func_info<R(Args...)> { };

    template<typename... Ts>
    struct is_same_all : public std::bool_constant<(std::is_same_v<first_t<Ts...>, Ts> && ...)> { };

    template<typename... Ts>
    inline constexpr bool is_same_all_v = is_same_all<Ts...>::value;

    template<typename Tp, typename... Ts>
    struct is_one_of : public std::bool_constant<(std::is_same_v<Tp, Ts> || ...)> { };

    template<typename Tp, typename... Ts>
    inline constexpr bool is_one_of_v = is_one_of<Tp, Ts...>::value;

    // **********************************

    template<class Tp, typename = void>
    struct is_iterable : public std::false_type { };

    template<class Tp>
    struct is_iterable<Tp, std::void_t<decltype(*std::declval<Tp>().begin()), decltype(std::declval<Tp>().end())>> : public std::true_type { };

    template<class Tp>
    inline constexpr bool is_iterable_v = is_iterable<Tp>::value;

    template<typename Tp, typename = void>
    struct is_deref : public std::false_type { };

    template<typename Tp>
    struct is_deref<Tp, std::void_t<decltype(*std::declval<Tp>())>> : public std::true_type { };

    template<class Tp>
    inline constexpr bool is_deref_v = is_deref<Tp>::value;
}
// namespace uf::mt
