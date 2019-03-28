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
    constexpr auto&& clone_something(Tp&& x)
    {
        return std::forward<Tp>(x);
    }

    template<auto, typename Tp>
    constexpr auto&& clone_something(Tp&& x)
    {
        return std::forward<Tp>(x);
    }

    template<typename, typename Tp>
    constexpr Tp clone_something();

    template<auto, typename Tp>
    constexpr Tp clone_something();

    namespace detail
    {
        template<typename T, typename N, typename... Ts>
        struct tuple_remove_last_helper
        {
            using type = typename tuple_remove_last_helper<T, constant<N::value - 1>, std::tuple_element_t<N::value - 1, T>, Ts...>::type;
        };

        template<typename T, typename... Ts>
        struct tuple_remove_last_helper<T, constant<0>, Ts...>
        {
            using type = std::tuple<Ts...>;
        };

        template<typename T, typename N, typename... Ts>
        struct tuple_remove_first_helper
        {
            using type = typename tuple_remove_first_helper<T, constant<N::value + 1>, Ts..., std::tuple_element_t<N::value, T>>::type;
        };

        template<typename T, typename... Ts>
        struct tuple_remove_first_helper<T, constant<std::tuple_size_v<T>>, Ts...>
        {
            using type = std::tuple<Ts...>;
        };

        template<typename Tp, typename S>
        struct tuple_clone_type_helper;

        template<typename Tp, auto... Ns>
        struct tuple_clone_type_helper<Tp, sequence<Ns...>>
        {
            using type = std::tuple<decltype(clone_something<Ns, Tp>())...>;
        };

        template<typename SCur, typename S, typename Sf, auto... Ns>
        struct seq_remove_helper;

        template<auto... SCurArgs, auto SArg, auto... SArgs, auto... Ns>
        struct seq_remove_helper<sequence<SCurArgs...>, sequence<SArg, SArgs...>, enif<((SArg == Ns) || ...)>, Ns...>
        {
            using type = typename seq_remove_helper<sequence<SCurArgs...>, sequence<SArgs...>, sfinae, Ns...>::type;
        };

        template<auto... SCurArgs, auto SArg, auto... SArgs, auto... Ns>
        struct seq_remove_helper<sequence<SCurArgs...>, sequence<SArg, SArgs...>, enif<((SArg != Ns) && ...)>, Ns...>
        {
            using type = typename seq_remove_helper<sequence<SCurArgs..., SArg>, sequence<SArgs...>, sfinae, Ns...>::type;
        };

        template<typename SCur, auto... Ns>
        struct seq_remove_helper<SCur, sequence<>, sfinae, Ns...>
        {
            using type = SCur;
        };
    }
    // namespace detail

    template<typename S, typename... Ts>
    struct tuple_from_seq;

    template<auto... Ns, typename... Ts>
    struct tuple_from_seq<sequence<Ns...>, Ts...>
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

    template<typename Tp>
    inline constexpr bool is_multi_v = is_pair_v<Tp> || is_tuple_v<Tp>;

    template<typename Tp>
    inline constexpr bool is_single_v = !is_multi_v<Tp>;

    template<typename Tp, typename Sf = sfinae>
    struct is_decayed : public std::false_type { };

    template<typename Tp>
    struct is_decayed<Tp, disif<std::is_const_v<Tp> || std::is_volatile_v<Tp> || std::is_reference_v<Tp>>> : public std::true_type { };

    template<typename Tp>
    inline constexpr bool is_decayed_v = is_decayed<Tp>::value;

    template<typename T, u64 N, enif<(std::tuple_size_v<T> >= N)> = sdef>
    struct tuple_remove_last_n
    {
        using type = typename detail::tuple_remove_last_helper<T, constant<std::tuple_size_v<T> - N>>::type;
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

    template<typename Tp, auto N>
    struct tuple_clone_type
    {
        using type = typename detail::tuple_clone_type_helper<Tp, make_sequence<N>>::type;
    };

    template<typename Tp, u64 N>
    using tuple_clone_type_t = typename tuple_clone_type<Tp, N>::type;

    template<u64 N, typename Tp, typename T>
    struct tuple_replace_index;

    template<u64 N, typename Tp, typename... Ts>
    struct tuple_replace_index<N, Tp, std::tuple<Ts...>>
    {
        using type = tuple_concat_t<tuple_from_seq_t<make_increasing_sequence<0, N>, Ts...>, std::tuple<Tp>, tuple_from_seq_t<make_increasing_sequence<N + 1, sizeof...(Ts)>, Ts...>>;
    };

    template<u64 N, typename Tp, typename T>
    using tuple_replace_index_t = typename tuple_replace_index<N, Tp, T>::type;

    template<typename S, auto... Ns>
    struct seq_remove
    {
        using type = typename detail::seq_remove_helper<sequence<>, S, sfinae, Ns...>::type;
    };

    template<typename S, auto... Ns>
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
    struct function_trait;

    template<class R, class... Args>
    struct function_trait<R(Args...)>
    {
        static constexpr u64 arity = sizeof...(Args);

        using result = R;

        template<u64 N>
        using nth = std::tuple_element_t<N, std::tuple<Args...>>;
    };

    template<class R, class... Args>
    struct function_trait<std::function<R(Args...)>> : public function_trait<R(Args...)> { };

    template<class R, class... Args>
    struct function_trait<R(*)(Args...)> : public function_trait<R(Args...)> { };

    template<class C, class R, class... Args>
    struct function_trait<R(C::*)(Args...)> : public function_trait<R(Args...)> { };

    template<typename... Ts>
    struct is_same_all : public std::bool_constant<(std::is_same_v<first_t<Ts...>, Ts> && ...)> { };

    template<typename... Ts>
    inline constexpr bool is_same_all_v = is_same_all<Ts...>::value;

    template<typename Tp, typename... Ts>
    struct is_one_of : public std::bool_constant<(std::is_same_v<Tp, Ts> || ...)> { };

    template<typename Tp, typename... Ts>
    inline constexpr bool is_one_of_v = is_one_of<Tp, Ts...>::value;

    template<typename S>
    struct is_positive_sequence;

    template<auto... Ns>
    struct is_positive_sequence<sequence<Ns...>> : public std::bool_constant<((Ns >= 0) && ...)> { };

    template<typename S>
    inline constexpr bool is_positive_sequence_v = is_positive_sequence<S>::value;

    template<typename Tp>
    struct is_smart_pointer
    {
        static const bool value = is_same_template_v<std::unique_ptr, Tp> ||
                                  is_same_template_v<std::shared_ptr, Tp> ||
                                  is_same_template_v<std::weak_ptr, Tp>;
    };

    template<class Tp>
    inline constexpr bool is_smart_pointer_v = is_smart_pointer<Tp>::value;

    template<typename Tp, typename = sfinae>
    struct is_iterator : public std::false_type { };

    template<typename Tp>
    struct is_iterator<Tp, sfinae_t<typename std::iterator_traits<Tp>::iterator_category>> : public std::true_type { };

    template<typename Tp, typename = sfinae>
    inline constexpr bool is_iterator_v = is_iterator<Tp>::value;

    // **********************************

    template<class Tp, typename = sfinae>
    struct is_iterable : public std::false_type { };

    template<class Tp>
    struct is_iterable<Tp, sfinae_t<decltype(*std::begin(std::declval<Tp>())), decltype(std::end(std::declval<Tp>()))>> : public std::true_type { };

    template<class Tp>
    inline constexpr bool is_iterable_v = is_iterable<Tp>::value;

    template<typename Tp, typename = sfinae>
    struct is_deref : public std::false_type { };

    template<typename Tp>
    struct is_deref<Tp, sfinae_t<decltype(*std::declval<Tp>())>> : public std::true_type { };

    template<class Tp>
    inline constexpr bool is_deref_v = is_deref<Tp>::value;
}
// namespace uf::mt
