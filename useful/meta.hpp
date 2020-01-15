#pragma once
#include "import.hpp"

#define DECLARE_N1(name, a1) \
    template<a1 A1> \
    inline constexpr auto name##_v = name<A1>::value

#define DECLARE_N2(name, a1, a2) \
    template<a1 A1, a2 A2> \
    inline constexpr auto name##_v = name<A1, A2>::value

#define DECLARE_N3(name, a1, a2, a3) \
    template<a1 A1, a2 A2, a3 A3> \
    inline constexpr auto name##_v = name<A1, A2, A3>::value

#define DECLARE_T1(name, a1) \
    template<a1 A1> \
    using name##_t = typename name<A1>::type

#define DECLARE_T2(name, a1, a2) \
    template<a1 A1, a2 A2> \
    using name##_t = typename name<A1, A2>::type

#define DECLARE_T3(name, a1, a2, a3) \
    template<a1 A1, a2 A2, a3 A3> \
    using name##_t = typename name<A1, A2, A3>::type

#define DECLARE_N1S(name, a1) \
    template<a1 A1> \
    inline constexpr auto name##_v = name<A1...>::value

#define DECLARE_N2S(name, a1, a2) \
    template<a1 A1, a2 A2> \
    inline constexpr auto name##_v = name<A1, A2...>::value

#define DECLARE_N3S(name, a1, a2, a3) \
    template<a1 A1, a2 A2, a3 A3> \
    inline constexpr auto name##_v = name<A1, A2, A3...>::value

#define DECLARE_T1S(name, a1) \
    template<a1 A1> \
    using name##_t = typename name<A1...>::type

#define DECLARE_T2S(name, a1, a2) \
    template<a1 A1, a2 A2> \
    using name##_t = typename name<A1, A2...>::type

#define DECLARE_T3S(name, a1, a2, a3) \
    template<a1 A1, a2 A2, a3 A3> \
    using name##_t = typename name<A1, A2, A3...>::type

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

    template<typename Tp>
    struct is_decayed : std::bool_constant<!std::is_const_v<Tp> && !std::is_volatile_v<Tp> && !std::is_reference_v<Tp>> { };

    DECLARE_N1(is_decayed, typename);

    template<typename F, typename... Ts>
    struct tpack_first : type_identity<F> { };

    DECLARE_T1S(tpack_first, typename...);

    template<auto N, auto... Ns>
    struct npack_first : constant<N> { };

    DECLARE_N2S(npack_first, auto, auto...);

    template<typename... Ts>
    struct tpack_last : type_identity<std::tuple_element_t<sizeof...(Ts) - 1, std::tuple<Ts...>>> { };

    DECLARE_T1S(tpack_last, typename...);

    template<auto... Ns>
    struct npack_last : constant<(Ns, ...)> { };

    DECLARE_N1S(npack_last, auto...);

    template<u64 N, typename... Ts>
    struct tpack_nth : type_identity<std::tuple_element_t<N, std::tuple<Ts...>>> { };

    DECLARE_T2S(tpack_nth, u64, typename...);

    template<u64 N, auto... Ns>
    struct npack_nth : constant<sequence<Ns...>::template nth<N>> { };

    DECLARE_N2S(npack_nth, u64, auto...);

    template<auto N, auto... Ns>
    struct is_npack_contain : std::bool_constant<((N == Ns) || ...)> { };

    DECLARE_N2S(is_npack_contain, auto, auto...);

    template<typename Tp, typename... Ts>
    struct is_tpack_contain : std::bool_constant<(std::is_same_v<Tp, Ts> || ...)> { };

    DECLARE_N2S(is_tpack_contain, typename, typename...);

    template<typename... Ts>
    struct is_tpack_same : std::bool_constant<(std::is_same_v<tpack_first_t<Ts...>, Ts> && ...)> { };

    DECLARE_N1S(is_tpack_same, typename...);

    template<auto... Ns>
    struct is_npack_same : std::bool_constant<((sequence<Ns...>::template value<0> == Ns) && ...)> { };

    DECLARE_N1S(is_npack_same, auto...);

    template<typename... Ts>
    struct is_tpack_different;

    template<typename First, typename... Ts>
    struct is_tpack_different<First, Ts...> : std::bool_constant<!(std::is_same_v<First, Ts> || ...) && is_tpack_different<Ts...>::value> { };

    template<typename First>
    struct is_tpack_different<First> : std::true_type { };

    template<>
    struct is_tpack_different<> : std::true_type { };

    DECLARE_N1S(is_tpack_different, typename...);

    inline namespace sequence_operations
    {        
        template<typename S>
        struct seq_first : constant<S::template get<0>> { };

        DECLARE_N1(seq_first, typename);

        template<typename S>
        struct seq_last : constant<S::template get<S::size - 1>> { };

        DECLARE_N1(seq_last, typename);

        template<typename S, u64... Ns>
        struct seq_select : type_identity<sequence<S::template value<Ns>...>> { };

        DECLARE_T2S(seq_select, typename, u64...);

        template<typename S1, typename S2>
        struct seq_select_seq;

        template<typename S1, auto... Ns>
        struct seq_select_seq<S1, sequence<Ns...>> : seq_select<S1, Ns...> { };

        DECLARE_T2(seq_select_seq, typename, typename);

        template<typename S, auto N>
        struct is_seq_contain;

        template<auto... Ns, auto N>
        struct is_seq_contain<sequence<Ns...>, N> : is_npack_contain<N, Ns...> { };

        DECLARE_N2(is_seq_contain, typename, auto);

        template<typename... Ts>
        struct seq_concat;

        template<>
        struct seq_concat<> : type_identity<sequence<>> { };

        template<auto... Ns>
        struct seq_concat<sequence<Ns...>> : type_identity<sequence<Ns...>> { };

        template<auto... Ns1, auto... Ns2, typename... Ts>
        struct seq_concat<sequence<Ns1...>,  sequence<Ns2...>, Ts...> : seq_concat<sequence<Ns1..., Ns2...>, Ts...> { };

        DECLARE_T1S(seq_concat, typename...);

        template<typename S, auto... Ns>
        struct seq_push_back;

        template<auto... SArgs, auto... Ns>
        struct seq_push_back<sequence<SArgs...>, Ns...> : type_identity<sequence<SArgs..., Ns...>> { };

        DECLARE_T2S(seq_push_back, typename, auto...);

        template<typename S, auto... Ns>
        struct seq_push_front;

        template<auto... SArgs, auto... Ns>
        struct seq_push_front<sequence<SArgs...>, Ns...> : type_identity<sequence<Ns..., SArgs...>> { };

        DECLARE_T2S(seq_push_front, typename, auto...);

        template<auto V, u64 N>
        struct seq_one_value : type_identity<seq_concat_t<seq_push_back_t<sequence<>, V>, typename seq_one_value<V, N - 1>::type>> { };

        template<auto V>
        struct seq_one_value<V, 0> : type_identity<sequence<>> { };

        DECLARE_T2(seq_one_value, auto, u64);

        template<typename S>
        struct seq_reverse;

        template<auto N, auto... Ns>
        struct seq_reverse<sequence<N, Ns...>>
        {
            using type = seq_push_back_t<typename seq_reverse<sequence<Ns...>>::type, N>;
        };

        template<>
        struct seq_reverse<sequence<>> : type_identity<sequence<>> { };

        DECLARE_T1(seq_reverse, typename);

        template<auto B, auto E>
        struct seq_increasing
        {
            static_assert (B < E);
            using type = seq_concat_t<sequence<B>, typename seq_increasing<B + 1, E>::type>;
        };

        template<auto E>
        struct seq_increasing<E, E> : type_identity<sequence<>> { };

        template<u64 N>
        using make_sequence = typename seq_increasing<u64(0), u64(N)>::type;

        DECLARE_T2(seq_increasing, auto, auto);

        template<auto B, auto E>
        struct seq_decreasing
        {
            static_assert (i64(B) >= i64(E - 1));
            using type = seq_reverse_t<seq_increasing_t<E, B + 1>>;
        };

        DECLARE_T2(seq_decreasing, auto, auto);

        template<typename S, u64 N>
        struct seq_remove_front
        {
            using type = seq_select_seq_t<S, seq_increasing_t<N, S::size>>;
        };

        DECLARE_T2(seq_remove_front, typename, u64);

        template<typename S, u64 N>
        struct seq_remove_back
        {
            using type = seq_select_seq_t<S, make_sequence<S::size - N>>;
        };

        DECLARE_T2(seq_remove_back, typename, u64);

        template<typename S, auto... Ns>
        struct seq_remove;

        template<auto Arg, auto... Args, auto... Ns>
        struct seq_remove<sequence<Arg, Args...>, Ns...>
        {
            using type = std::conditional_t<((Arg == Ns) || ...),
                                            typename seq_remove<sequence<Args...>, Ns...>::type,
                                            seq_push_front_t<typename seq_remove<sequence<Args...>, Ns...>::type, Arg>>;
        };

        template<auto... Ns>
        struct seq_remove<sequence<>, Ns...> : type_identity<sequence<>> { };

        DECLARE_T2S(seq_remove, typename, auto...);
    }
    // inline namespace sequence_operations

    inline namespace tuple_operations
    {
        template<typename T>
        struct tuple_first : type_identity<std::tuple_element_t<0, T>> { };

        DECLARE_T1(tuple_first, typename);

        template<typename T>
        struct tuple_last : type_identity<std::tuple_element_t<std::tuple_size_v<T> - 1, T>> { };

        DECLARE_T1(tuple_last, typename);

        template<typename T, u64... Ns>
        struct tuple_select : type_identity<std::tuple<std::tuple_element_t<Ns, T>...>> { };

        DECLARE_T2S(tuple_select, typename, u64...);

        template<typename T, typename S>
        struct tuple_select_seq;

        template<typename T, auto... Ns>
        struct tuple_select_seq<T, sequence<Ns...>> : tuple_select<T, Ns...> { };

        DECLARE_T2(tuple_select_seq, typename, typename);

        template<typename T, typename Tp>
        struct is_tuple_contain;

        template<typename... Ts, typename Tp>
        struct is_tuple_contain<std::tuple<Ts...>, Tp> : is_tpack_contain<Tp, Ts...> { };

        DECLARE_N2(is_tuple_contain, typename, typename);

        template<typename... Ts>
        struct tuple_concat;

        template<>
        struct tuple_concat<> : type_identity<std::tuple<>> { };

        template<typename... Ts>
        struct tuple_concat<std::tuple<Ts...>> : type_identity<std::tuple<Ts...>> { };

        template<typename... Ts1, typename... Ts2, typename... Ts>
        struct tuple_concat<std::tuple<Ts1...>, std::tuple<Ts2...>, Ts...> : tuple_concat<std::tuple<Ts1..., Ts2...>, Ts...> { };

        DECLARE_T1S(tuple_concat, typename...);

        template<typename T, typename... Ts>
        struct tuple_push_back;

        template<typename... TArgs, typename... Ts>
        struct tuple_push_back<std::tuple<TArgs...>, Ts...> : type_identity<std::tuple<TArgs..., Ts...>> { };

        DECLARE_T2S(tuple_push_back, typename, typename...);

        template<typename T, typename... Ts>
        struct tuple_push_front;

        template<typename... TArgs, typename... Ts>
        struct tuple_push_front<std::tuple<TArgs...>, Ts...>
        {
            using type = std::tuple<Ts..., TArgs...>;
        };

        DECLARE_T2S(tuple_push_front, typename, typename...);

        template<typename T, u64 N>
        struct tuple_one_type : type_identity<tuple_concat_t<tuple_push_back_t<std::tuple<>, T>, typename tuple_one_type<T, N - 1>::type>> { };

        template<typename T>
        struct tuple_one_type<T, 0> : type_identity<std::tuple<>> { };

        DECLARE_T2(tuple_one_type, typename, u64);

        template<typename T>
        struct tuple_reverse;

        template<typename T1, typename... Ts>
        struct tuple_reverse<std::tuple<T1, Ts...>>
        {
            using type = tuple_push_back_t<typename tuple_reverse<std::tuple<Ts...>>::type, T1>;
        };

        DECLARE_T1(tuple_reverse, typename);

        template<typename T, u64 N>
        struct tuple_remove_front
        {
            using type = tuple_select_seq_t<T, seq_increasing_t<N, std::tuple_size_v<T>>>;
        };

        DECLARE_T2(tuple_remove_front, typename, u64);

        template<typename T, u64 N>
        struct tuple_remove_back
        {
            using type = tuple_select_seq_t<T, make_sequence<std::tuple_size_v<T> - N>>;
        };

        DECLARE_T2(tuple_remove_back, typename, u64);

        template<typename T, typename... Ts>
        struct tuple_remove;

        template<typename Arg, typename... Args, typename... Ts>
        struct tuple_remove<std::tuple<Arg, Args...>, Ts...>
        {
            using type = std::conditional_t<((std::is_same_v<Arg, Ts>) || ...),
                                            typename tuple_remove<std::tuple<Args...>, Ts...>::type,
                                            tuple_push_front_t<typename tuple_remove<std::tuple<Args...>, Ts...>::type, Arg>>;
        };

        template<typename... Ts>
        struct tuple_remove<std::tuple<>, Ts...> : type_identity<std::tuple<>> { };

        DECLARE_T2S(tuple_remove, typename, typename...);
    }
    // inline namespace tuple_operations

    template<typename Tp>
    struct is_instantiated : std::false_type { };

    template<template<typename...> typename Tp, typename... Ts>
    struct is_instantiated<Tp<Ts...>> : std::true_type { };

    DECLARE_N1(is_instantiated, typename);

    template<typename A, typename B>
    struct is_instantiated_from_same : std::false_type { };

    template<template<typename...> typename Tp, typename... A, typename... B>
    struct is_instantiated_from_same<Tp<A...>, Tp<B...>> : std::true_type { };

    DECLARE_N2(is_instantiated_from_same, typename, typename);

    template<template<typename...> typename Expected, typename Tested>
    struct is_instantiated_from : std::false_type { };

    template<template<typename...> typename Expected, typename... Ts>
    struct is_instantiated_from<Expected, Expected<Ts...>> : std::true_type { };

    DECLARE_N2(is_instantiated_from, template<typename...> typename, typename);

    template<typename Tp>
    struct instance_info;

    template<template<typename...> typename Tp, typename... Ts>
    struct instance_info<Tp<Ts...>>
    {
        static constexpr u64 args = sizeof...(Ts);

        template<typename... Ts_>
        using type = Tp<Ts_...>;

        template<u64 N>
        using nth = std::tuple_element_t<N, std::tuple<Ts...>>;
    };

    template<template<typename...> typename Template, typename T>
    struct instance_from_tuple;

    template<template<typename...> typename Template, typename... Ts>
    struct instance_from_tuple<Template, std::tuple<Ts...>> : type_identity<Template<Ts...>> { };

    DECLARE_T2(instance_from_tuple, template<typename...> typename, typename);

    template<typename F>
    struct function_info;

    template<typename R, typename... Args>
    struct function_info<R(Args...)>
    {
        static constexpr u64 arity = sizeof...(Args);

        using result = R;

        template<u64 N>
        using nth = std::tuple_element_t<N, std::tuple<Args...>>;
    };

    template<typename R, typename... Args>
    struct function_info<std::function<R(Args...)>> : function_info<R(Args...)> { };

    template<typename R, typename... Args>
    struct function_info<R(*)(Args...)> : function_info<R(Args...)> { };

    template<typename C, typename R, typename... Args>
    struct function_info<R(C::*)(Args...)> : function_info<R(Args...)> { };

    // **********************************
    template<class Tp, typename = sfinae>
    struct is_iterable : std::false_type { };

    template<class Tp>
    struct is_iterable<Tp, sfinae_t<decltype(*std::begin(std::declval<Tp>())), decltype(std::end(std::declval<Tp>()))>> : std::true_type { };

    DECLARE_N1(is_iterable, typename);

    template<typename Tp, typename = sfinae>
    struct is_dereferenceable : std::false_type { };

    template<typename Tp>
    struct is_dereferenceable<Tp, sfinae_t<decltype(*std::declval<Tp>())>> : std::true_type { };

    DECLARE_N1(is_dereferenceable, typename);

    // **********************************
    template<typename S>
    struct is_sequence : std::false_type { };

    template<auto... Ns>
    struct is_sequence<sequence<Ns...>> : std::true_type { };

    DECLARE_N1(is_sequence, typename);

    template<typename S>
    struct is_positive_sequence : std::false_type { };

    template<auto... Ns>
    struct is_positive_sequence<sequence<Ns...>> : std::bool_constant<((Ns >= 0) && ...)> { };

    DECLARE_N1(is_positive_sequence, typename);

    template<typename Tp>
    struct is_smart_pointer : std::bool_constant<is_instantiated_from_v<std::unique_ptr, Tp> || is_instantiated_from_v<std::shared_ptr, Tp> || is_instantiated_from_v<std::weak_ptr, Tp>> { };

    DECLARE_N1(is_smart_pointer, typename);

    template<typename Tp, typename = sfinae>
    struct is_iterator : std::false_type { };

    template<typename Tp>
    struct is_iterator<Tp, sfinae_t<typename std::iterator_traits<Tp>::iterator_category>> : std::true_type { };

    DECLARE_N1(is_iterator, typename);

    template<typename Tp>
    struct is_reverse_iterator : std::bool_constant<is_instantiated_from_v<std::reverse_iterator, Tp>> { };

    DECLARE_N1(is_reverse_iterator, typename);

    template<typename Tp>
    struct is_not_reverse_iterator : std::bool_constant<is_iterator_v<Tp> && !is_reverse_iterator_v<Tp>> { };

    DECLARE_N1(is_not_reverse_iterator, typename);

    template<typename Tp, typename = sfinae>
    struct is_random_access_iterator : std::false_type { };

    template<typename Tp>
    struct is_random_access_iterator<Tp, enif<std::is_same_v<typename std::iterator_traits<Tp>::iterator_category, std::random_access_iterator_tag>>> : std::true_type { };

    DECLARE_N1(is_random_access_iterator, typename);

    template<typename Tp, typename = sfinae>
    struct is_random_access_container : std::false_type { };

    template<typename Tp>
    struct is_random_access_container<Tp, enif<is_iterable_v<Tp> && is_random_access_iterator_v<typename std::remove_reference_t<Tp>::iterator>>> : std::true_type { };

    DECLARE_N1(is_random_access_container, typename);
}
// namespace uf::mt

namespace uf
{
    using mt::make_sequence;
}
// namespace uf

#undef DECLARE_N1
#undef DECLARE_N2
#undef DECLARE_N3
#undef DECLARE_T1
#undef DECLARE_T2
#undef DECLARE_T3
#undef DECLARE_N1S
#undef DECLARE_N2S
#undef DECLARE_N3S
#undef DECLARE_T1S
#undef DECLARE_T2S
#undef DECLARE_T3S
