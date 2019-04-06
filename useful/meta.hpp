#pragma once
#include "import.hpp"

#define DECLARE_V1(name, a1) \
    template<a1 A1> \
    inline constexpr auto name##_v = name<A1>::value

#define DECLARE_V2(name, a1, a2) \
    template<a1 A1, a2 A2> \
    inline constexpr auto name##_v = name<A1, A2>::value

#define DECLARE_V3(name, a1, a2, a3) \
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

#define DECLARE_V1S(name, a1) \
    template<a1 A1> \
    inline constexpr auto name##_v = name<A1...>::value

#define DECLARE_V2S(name, a1, a2) \
    template<a1 A1, a2 A2> \
    inline constexpr auto name##_v = name<A1, A2...>::value

#define DECLARE_V3S(name, a1, a2, a3) \
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

    template<auto N, auto... Ns>
    struct is_npack_contain : std::bool_constant<((N == Ns) || ...)> { };

    template<auto N, auto... Ns>
    inline constexpr bool is_npack_contain_v = is_npack_contain<N, Ns...>::value;

    template<typename Tp, typename... Ts>
    struct is_tpack_contain : std::bool_constant<(std::is_same_v<Tp, Ts> || ...)> { };

    template<typename Tp, typename... Ts>
    inline constexpr bool is_tpack_contain_v = is_tpack_contain<Tp, Ts...>::value;

    inline namespace sequence_operations
    {
        template<typename S1, typename S2>
        struct seq_unite;

        template<auto... Ns1, auto... Ns2>
        struct seq_unite<sequence<Ns1...>,  sequence<Ns2...>> : type_identity<sequence<Ns1..., Ns2...>> { };

        DECLARE_T2(seq_unite, typename, typename);

        template<auto N, typename S>
        struct seq_contain;

        template<auto N, auto... Ns>
        struct seq_contain<N, sequence<Ns...>> : is_npack_contain<N, Ns...> { };

        DECLARE_V2(seq_contain, auto, typename);

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

        template<typename S, auto... Ns>
        using seq_push_front_t = typename seq_push_front<S, Ns...>::type;

        template<u64 N, typename S>
        struct seq_remove_front;

        template<u64 N, auto Arg, auto... Args>
        struct seq_remove_front<N, sequence<Arg, Args...>>
        {
            using type = typename seq_remove_front<N - 1, sequence<Args...>>::type;
        };

        template<auto... Args>
        struct seq_remove_front<0, sequence<Args...>>
        {
            using type = sequence<Args...>;
        };

        DECLARE_T2(seq_remove_front, u64, typename);

        template<typename S>
        struct seq_reverse;

        template<auto N, auto... Ns>
        struct seq_reverse<sequence<N, Ns...>>
        {
            using type = seq_push_back_t<typename seq_reverse<sequence<Ns...>>::type, N>;
        };

        template<>
        struct seq_reverse<sequence<>>
        {
            using type = sequence<>;
        };

        DECLARE_T1(seq_reverse, typename);

        template<u64 N, typename S>
        struct seq_remove_back
        {
            using type = seq_reverse_t<seq_remove_front_t<N, seq_reverse_t<S>>>;
        };

        DECLARE_T2(seq_remove_back, u64, typename);

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

    }
    // inline namespace tuple_operations

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

    template<typename Tp>
    struct is_template : public std::false_type { };

    template<template<typename...> typename Tp, typename... Ts>
    struct is_template<Tp<Ts...>> : public std::true_type { };

    template<typename Tp>
    inline constexpr bool is_template_v = is_template<Tp>::value;

    template<typename A, typename B>
    struct is_same_template : public std::false_type { };

    template<template<typename...> typename Tp, typename... A, typename... B>
    struct is_same_template<Tp<A...>, Tp<B...>> : public std::true_type { };

    template<typename A, typename B>
    inline constexpr bool is_same_template_v = is_same_template<A, B>::value;

    template<template<typename...> typename Expected, typename Tested>
    struct is_instantiated_from : public std::false_type { };

    template<template<typename...> typename Expected, typename... Ts>
    struct is_instantiated_from<Expected, Expected<Ts...>> : public std::true_type { };

    template<template<typename...> typename Expected, typename Tested>
    inline constexpr bool is_instantiated_from_v =  is_instantiated_from<Expected, Tested>::value;

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

    // **********************************
    template<typename S>
    struct is_positive_sequence : std::false_type { };

    template<auto... Ns>
    struct is_positive_sequence<sequence<Ns...>> : public std::bool_constant<((Ns >= 0) && ...)> { };

    template<typename S>
    inline constexpr bool is_positive_sequence_v = is_positive_sequence<S>::value;

    template<typename Tp>
    struct is_smart_pointer
    {
        static const bool value = is_instantiated_from_v<std::unique_ptr, Tp> || is_instantiated_from_v<std::shared_ptr, Tp> || is_instantiated_from_v<std::weak_ptr, Tp>;
    };

    template<class Tp>
    inline constexpr bool is_smart_pointer_v = is_smart_pointer<Tp>::value;

    template<typename Tp, typename = sfinae>
    struct is_iterator : public std::false_type { };

    template<typename Tp>
    struct is_iterator<Tp, sfinae_t<typename std::iterator_traits<Tp>::iterator_category>> : public std::true_type { };

    template<typename Tp, typename = sfinae>
    inline constexpr bool is_iterator_v = is_iterator<Tp>::value;

    template<typename Tp, typename = sfinae>
    struct is_random_access_iterator : public std::false_type { };

    template<typename Tp>
    struct is_random_access_iterator<Tp, enif<std::is_same_v<typename std::iterator_traits<Tp>::iterator_category, std::random_access_iterator_tag>>> : public std::true_type { };

    template<typename Tp>
    inline constexpr bool is_random_access_iterator_v = is_random_access_iterator<Tp>::value;

    template<typename Tp, typename = sfinae>
    struct is_random_access_container : std::false_type { };

    template<typename Tp>
    struct is_random_access_container<Tp, enif<is_iterable_v<Tp>>> : public is_random_access_iterator<typename std::remove_reference_t<Tp>::iterator> { };

    template<typename Tp>
    inline constexpr bool is_random_access_container_v = is_random_access_container<Tp>::value;

}
// namespace uf::mt

#undef DECLARE_V1
#undef DECLARE_V2
#undef DECLARE_V3
#undef DECLARE_T1
#undef DECLARE_T2
#undef DECLARE_T3
#undef DECLARE_V1S
#undef DECLARE_V2S
#undef DECLARE_V3S
#undef DECLARE_T1S
#undef DECLARE_T2S
#undef DECLARE_T3S
