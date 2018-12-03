#include <vector>
#include <functional>

namespace uf
{
    namespace basic_types
    {
        using u8 = uint8_t;
        using i8 = int8_t;

        using u16 = uint16_t;
        using i16 = int16_t;

        using u32 = uint32_t;
        using i32 = int32_t;

        using u64 = uint64_t;
        using i64 = int64_t;

        namespace literals
        {
            inline constexpr u8 operator"" _u8(unsigned long long int value) noexcept { return value; }
            inline constexpr i8 operator"" _i8(unsigned long long int value) noexcept { return value; }

            inline constexpr u16 operator"" _u16(unsigned long long int value) noexcept { return value; }
            inline constexpr i16 operator"" _i16(unsigned long long int value) noexcept { return value; }

            inline constexpr u32 operator"" _u32(unsigned long long int value) noexcept { return value; }
            inline constexpr i32 operator"" _i32(unsigned long long int value) noexcept { return value; }

            inline constexpr u64 operator"" _u64(unsigned long long int value) noexcept { return value; }
            inline constexpr i64 operator"" _i64(unsigned long long int value) noexcept { return value; }
        }
        // end namespace literals
    }
    // end namespace basic_types

    using namespace basic_types;
    using namespace basic_types::literals;

    using std::vector;
    using std::string;
    using std::string_view;
    using std::pair;
    using std::function;

    using std::enable_if_t;
    using std::true_type;
    using std::false_type;
    using std::remove_reference_t;
    using std::remove_pointer_t;
    using std::is_lvalue_reference_v;
    using std::conditional_t;
    using std::is_pointer_v;
    using std::is_same_v;
    using std::decay_t;
    using std::integral_constant;
    using std::tuple;
    using std::tuple_element_t;
    using std::tuple_size_v;

    namespace meta
    {
        namespace detail
        {
            template<class Tp>
            struct is_string_type_helper : public conditional_t<is_pointer_v<Tp>,
                                                      conditional_t<is_same_v<decay_t<remove_pointer_t<Tp>>, char>,
                                                          true_type,
                                                          false_type>,
                                                      false_type> { };

            template<>
            struct is_string_type_helper<string> : public true_type { };

            template<>
            struct is_string_type_helper<string_view> : public true_type { };

            template<typename Tuple, typename N, typename... Types>
            struct tuple_remove_last_helper
            {
                using type = typename tuple_remove_last_helper<Tuple, integral_constant<size_t, N::value - 1u>, tuple_element_t<N::value - 1u, Tuple>, Types...>::type;
            };

            template<typename Tuple, typename... Types>
            struct tuple_remove_last_helper<Tuple, integral_constant<size_t, 0u>, Types...>
            {
                using type = tuple<Types...>;
            };

            template<typename Tuple, typename N, typename... Ts>
            struct tuple_remove_first_helper
            {
                using type = typename tuple_remove_first_helper<Tuple, integral_constant<size_t, N::value + 1u>, Ts..., tuple_element_t<N::value, Tuple>>::type;
            };

            template<typename Tuple, typename... Ts>
            struct tuple_remove_first_helper<Tuple, integral_constant<size_t, tuple_size_v<Tuple>>, Ts...>
            {
                using type = tuple<Ts...>;
            };

            template<typename TargetWrapper, typename Tuple, size_t N, typename... Types>
            struct tuple_apply_to_target_helper
            {
                using type = typename tuple_apply_to_target_helper<TargetWrapper, Tuple, N - 1u, tuple_element_t<N - 1u, Tuple>, Types...>::type;
            };

            template<typename TargetWrapper, typename Tuple, typename... Types>
            struct tuple_apply_to_target_helper<TargetWrapper, Tuple, 0u, Types...>
            {
                using type = typename TargetWrapper::template type<Types...>;
            };

            template<typename TupleA, typename IndexA, typename TupleB, typename IndexB, typename... Types>
            struct tuple_concat_two_helper : public tuple_concat_two_helper<TupleA, integral_constant<size_t, IndexA::value + 1u>,
                    TupleB, integral_constant<size_t, IndexB::value>,
                    Types..., tuple_element_t<IndexA::value, TupleA>> { };


            template<typename TupleA, typename TupleB, typename IndexB, typename... Types>
            struct tuple_concat_two_helper<TupleA, integral_constant<size_t, tuple_size_v<TupleA>>, TupleB, IndexB, Types...> : public
                    tuple_concat_two_helper<TupleA, integral_constant<size_t, tuple_size_v<TupleA>>,
                    TupleB, integral_constant<size_t, IndexB::value + 1u>,
                    Types..., tuple_element_t<IndexB::value, TupleB>> { };

            template<typename TupleA, typename TupleB, typename... Types>
            struct tuple_concat_two_helper<TupleA, integral_constant<size_t, tuple_size_v<TupleA>>, TupleB,
                                           integral_constant<size_t, tuple_size_v<TupleB>>, Types...>
            {
                using type = tuple<Types...>;
            };

            template<typename TupleA, typename TupleB>
            struct tuple_concat_two
            {
                using type = typename tuple_concat_two_helper<TupleA, integral_constant<size_t, 0u>, TupleB, integral_constant<size_t, 0u>>::type;
            };

            template<typename Result, typename Tuple, typename... Tuples>
            struct tuple_concat_helper : public tuple_concat_helper<typename tuple_concat_two<Result, Tuple>::type, Tuples...> { };

            template<typename Result, typename Tuple>
            struct tuple_concat_helper<Result, Tuple>
            {
                using type = typename tuple_concat_two<Result, Tuple>::type;
            };

            template<typename Tuple, size_t N, typename... Types>
            struct tuple_push_back_helper : public tuple_push_back_helper<Tuple, N - 1u, tuple_element_t<N - 1u, Tuple>, Types...> { };

            template<typename Tuple, typename... Types>
            struct tuple_push_back_helper<Tuple, 0u, Types...>
            {
                using type = tuple<Types...>;
            };

            template<typename Tuple, typename N, typename... Types>
            struct tuple_push_front_helper : public tuple_push_front_helper<Tuple, integral_constant<size_t, N::value + 1u>,
                    Types..., tuple_element_t<N::value, Tuple>> { };

            template<typename Tuple, typename... Types>
            struct tuple_push_front_helper<Tuple, integral_constant<size_t, tuple_size_v<Tuple>>, Types...>
            {
                using type = tuple<Types...>;
            };

            template<typename Tp, size_t N, typename... Types>
            struct tuple_same_n_helper
            {
                using type = typename tuple_same_n_helper<Tp, N - 1u, Tp, Types...>::type;
            };

            template<typename Tp, typename... Types>
            struct tuple_same_n_helper<Tp, 0u, Types...>
            {
                using type = tuple<Types...>;
            };
        }
        // end namespace detail

        template<class Tp>
        struct is_string_type : public detail::is_string_type_helper<decay_t<Tp>> { };

        template<class Tp>
        inline constexpr bool is_string_type_v = is_string_type<Tp>::value;

        template<template<typename...> typename Target>
        struct tmpl_tmpl_wrapper
        {
            template<typename... Types>
            using type = Target<Types...>;
        };

        template<template<typename...> typename Target, typename... Ts>
        using tmpl_tmpl_wrapper_t = typename tmpl_tmpl_wrapper<Target>::template type<Ts...>;

        template<typename Tuple, typename = enable_if_t<(tuple_size_v<Tuple> >= 1u)>>
        struct tuple_remove_last
        {
            using type = typename detail::tuple_remove_last_helper<Tuple, integral_constant<size_t, tuple_size_v<Tuple> - 1u>>::type;
        };

        template<typename Tuple>
        using tuple_remove_last_t = typename tuple_remove_last<Tuple>::type;

        template<typename Tuple, size_t N, typename = enable_if_t<(tuple_size_v<Tuple> >= N)>>
        struct tuple_remove_last_n
        {
            using type = typename detail::tuple_remove_last_helper<Tuple, integral_constant<size_t, tuple_size_v<Tuple> - N>>::type;
        };

        template<typename Tuple, size_t N>
        using tuple_remove_last_n_t = typename tuple_remove_last_n<Tuple, N>::type;

        template<typename Tuple, typename = enable_if_t<(tuple_size_v<Tuple> >= 1)>>
        struct tuple_remove_first
        {
            using type = typename detail::tuple_remove_first_helper<Tuple, integral_constant<size_t, 1u>>::type;
        };

        template<typename Tuple>
        using tuple_remove_first_t = typename tuple_remove_first<Tuple>::type;

        template<typename Tuple, size_t N, typename = enable_if_t<(tuple_size_v<Tuple> >= N)>>
        struct tuple_remove_first_n
        {
            using type = typename detail::tuple_remove_first_helper<Tuple, integral_constant<size_t, N>>::type;
        };

        template<typename Tuple, size_t N>
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
            using type = typename detail::tuple_push_front_helper<Tuple, integral_constant<size_t, 0u>, Types...>::type;
        };

        template<typename Tuple, typename... Types>
        using tuple_push_front_t = typename tuple_push_front<Tuple, Types...>::type;

        template<template<typename...> typename Target, typename Tuple>
        struct tuple_apply_to_target
        {
            using type = typename detail::tuple_apply_to_target_helper<tmpl_tmpl_wrapper<Target>, Tuple, tuple_size_v<Tuple>>::type;
        };

        template<template<typename...> typename Target, typename Tuple>
        using tuple_apply_to_target_t = typename tuple_apply_to_target<Target, Tuple>::type;

        template<template<typename...> typename Target, typename Tuple, size_t... Indexes>
        using tuple_apply_to_target_index_t = Target<tuple_element_t<Indexes, Tuple>...>;

        template<typename... Tuples>
        struct tuple_concat
        {
            using type = typename detail::tuple_concat_helper<tuple<>, Tuples...>::type;
        };

        template<typename... Tuples>
        using tuple_concat_t = typename tuple_concat<Tuples...>::type;

        template<typename Tp, size_t N>
        struct tuple_same_n
        {
            using type = typename detail::tuple_same_n_helper<Tp, N>::type;
        };

        template<typename Tp, size_t N>
        using tuple_same_n_t = typename tuple_same_n<Tp, N>::type;

        template<class Function>
        struct function_assistant;

        template<class R, class... Args>
        struct function_assistant<function<R(Args...)>>
        {
            static constexpr size_t arg_n = sizeof...(Args);

            using res_t = R;

            template<size_t N>
            using arg_t = tuple_element_t<N, tuple<Args...>>;
        };
    }
    // end namespace meta

    namespace detail
    {
        template<class Container, class Bounds>
        auto create_split_vector(Bounds&& bounds)
        {
            vector<Container> result;
            result.reserve(bounds.size());
            for (const auto& [b, e] : bounds)
                result.emplace_back(b, e);
            return result;
        }
    }
    // end namesapce detail

    template<typename UContainer, typename E>
    constexpr auto&& fwd_elem(E&& e)
    {
        if constexpr (!is_lvalue_reference_v<E>)
            return static_cast<remove_reference_t<E>&&>(e);
        else if constexpr (is_lvalue_reference_v<UContainer>)
            return static_cast<remove_reference_t<E>&>(e);
        else
            return static_cast<remove_reference_t<E>&&>(e);
    }

    template<class Tp, template<class> class Compare>
    class dereference_compare
    {
        const Compare<Tp> comp_;

    public:
        template<class Pointer>
        bool operator()(const Pointer& a, const Pointer& b) const
        {
            return comp_(*a, *b);
        }
    };

    template<class Container, class... Delimiters>
    auto split(Container&& container, Delimiters&&... delimiters)
    {
        using container_t = remove_reference_t<Container>;
        using iterator_t = typename container_t::const_iterator;

        bool first = true;
        vector<pair<iterator_t, iterator_t>> bounds;
        for (auto i = container.cbegin(); i != container.cend(); ++i)
        {
            if (first)
            {
                if (((*i == delimiters) || ...))
                    continue;
                first = false;
                bounds.push_back({i, container.cend()});
                continue;
            }
            if (((*i == delimiters) || ...))
            {
                bounds.back().second = i;
                first = true;
            }
        }

        return detail::create_split_vector<container_t>(bounds);
    }

        /*
        template<class Container, class DelimContainer>
        auto split_with_container(Container&& container, DelimContainer&& delimiters)
        {
            bool first = true;
            split_result<Container> result;
            for (auto i = container.cbegin(); i < container.cend(); ++i)
            {
                if (first)
                {
                    for (const auto& d : delimiters)
                        if (d == *i)
                            goto NEXT;
                    first = false;
                    result.bounds.push_back({i, container.end()});
                    continue;
                }
                for (const auto& d : delimiters)
                {
                    if (d == *i)
                    {
                        result.bounds.back().second = i;
                        first = true;
                        goto NEXT;
                    }
                }
            NEXT: ;
            }
            return result;
        }

        template<class Container, class DelimSetContainer>
        auto split_with_set(Container&& container, DelimSetContainer&& delimiters)
        {
            bool first = true;
            split_result<Container> result;
            for (auto i = container.cbegin(); i < container.cend(); ++i)
            {
                if (first)
                {
                    if (delimiters.count(*i))
                        continue;
                    first = false;
                    result.bounds.push_back({i, container.end()});
                    continue;
                }
                if (delimiters.count(*i))
                {
                    result.bounds.back().second = i;
                    first = true;
                }
            }
            return result;
        }

        template<class Container, class Function>
        auto split_with_function(Container&& container, Function&& delim_func)
        {
            size_t index = 0;
            bool first = true;
            split_result<Container> result;
            for (auto i = container.begin(); i < container.end(); ++i, ++index)
            {
                if (first)
                {
                    bool fres;
                    if constexpr (function_assistant<std::remove_reference_t<Function>>::arg_n == 1)
                        fres = delim_func(*i);
                    else
                        fres = delim_func(*i, index);
                    if (fres)
                        continue;
                    first = false;
                    result.bounds.push_back({i, container.end()});
                    continue;
                }
                if (delim_func(*i, index))
                {
                    result.bounds.back().second = i;
                    first = true;
                }
            }
            return result;
        }
        */
}
// end namespace uf
