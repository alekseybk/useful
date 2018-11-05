#include <thread>
#include <future>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include <unordered_set>

using namespace std;

// https://github.com/elseecay/useful

#define UFER(what, ...) uf::err(what, __PRETTY_FUNCTION__, ##__VA_ARGS__)

namespace uf
{
    namespace internal
    {
        template<class>
        struct is_string_type_helper : public std::false_type {};

        template<>
        struct is_string_type_helper<char*> : public std::true_type {};

        template<>
        struct is_string_type_helper<const char*> : public std::true_type {};

        template<>
        struct is_string_type_helper<std::string> : public std::true_type {};

        template<>
        struct is_string_type_helper<std::string_view> : public std::true_type {};

        template<typename Tuple, typename N, typename... Types>
        struct remove_last_in_tuple_helper : public remove_last_in_tuple_helper<Tuple, integral_constant<size_t, N::value - 1u>, tuple_element_t<N::value - 1u, Tuple>, Types...> { };

        template<typename Tuple, typename... Types>
        struct remove_last_in_tuple_helper<Tuple, integral_constant<size_t, 0u>, Types...>
        {
            using type = std::tuple<Types...>;
        };

        template<typename Box, typename Tuple, size_t N, typename... Types>
        struct tuple_types_on_target_helper : public tuple_types_on_target_helper<Box, Tuple, N - 1u, tuple_element_t<N - 1u, Tuple>, Types...> { };

        template<typename Box, typename Tuple, typename... Types>
        struct tuple_types_on_target_helper<Box, Tuple, 0u, Types...>
        {
            using type = typename Box::template type<Types...>;
        };
    }

    using namespace internal;

    template<class Tp>
    struct is_string_type : public is_string_type_helper<std::decay_t<Tp>> {};

    template<class Tp>
    inline constexpr bool is_string_type_v = is_string_type<Tp>::value;

    template<typename Tuple, typename = enable_if_t<(tuple_size<Tuple>::value >= 1u)>>
    struct remove_last_one_in_tuple
    {
        using type = typename remove_last_in_tuple_helper<Tuple, integral_constant<size_t, tuple_size<Tuple>::value - 1u>>::type;
    };

    template<typename Tuple>
    using remove_last_one_in_tuple_t = typename remove_last_one_in_tuple<Tuple>::type;

    template<typename Tuple, size_t N, typename = enable_if_t<(tuple_size<Tuple>::value >= N)>>
    struct remove_last_n_in_tuple
    {
        using type = typename remove_last_in_tuple_helper<Tuple, integral_constant<size_t, tuple_size<Tuple>::value - N>>::type;
    };

    template<typename Tuple, size_t N>
    using remove_last_n_in_tuple_t = typename remove_last_n_in_tuple<Tuple, N>::type;

    template<typename Tuple, size_t N, typename... Types>
    struct add_to_tuple_end_helper : public add_to_tuple_end_helper<Tuple, N - 1u, tuple_element_t<N - 1u, Tuple>, Types...> { };

    template<typename Tuple, typename... Types>
    struct add_to_tuple_end_helper<Tuple, 0u, Types...>
    {
        using type = tuple<Types...>;
    };

    template<typename Tuple, typename... Types>
    struct add_to_tuple_end
    {
        using type = typename add_to_tuple_end_helper<Tuple, tuple_size<Tuple>::value, Types...>::type;
    };

    template<typename Tuple, typename... Types>
    using add_to_tuple_end_t = typename add_to_tuple_end<Tuple, Types...>::type;

    template<typename Tuple, typename N, typename... Types>
    struct add_to_tuple_begin_helper : public add_to_tuple_begin_helper<Tuple, integral_constant<size_t, N::value + 1u>,
    Types..., tuple_element_t<N::value, Tuple>> { };

    template<typename Tuple, typename... Types>
    struct add_to_tuple_begin_helper<Tuple, integral_constant<size_t, tuple_size<Tuple>::value>, Types...>
    {
        using type = tuple<Types...>;
    };

    template<typename Tuple, typename... Types>
    struct add_to_tuple_begin
    {
        using type = typename add_to_tuple_begin_helper<Tuple, integral_constant<size_t, 0u>, Types...>::type;
    };

    template<typename Tuple, typename... Types>
    using add_to_tuple_begin_t = typename add_to_tuple_begin<Tuple, Types...>::type;

    template<template<typename...> typename Target>
    struct tmpl_tmpl_box
    {
        template<typename... Types>
        using type = Target<Types...>;
    };

    template<template<typename...> typename Target, typename Tuple>
    struct tuple_types_on_target
    {
        using type = typename tuple_types_on_target_helper<tmpl_tmpl_box<Target>, Tuple, tuple_size<Tuple>::value>::type;
    };

    template<template<typename...> typename Target, typename Tuple>
    using tuple_types_on_target_t = typename tuple_types_on_target<Target, Tuple>::type;

    template<template<typename...> typename Target, typename Tuple, size_t... Indexes>
    using tuple_types_on_target_idx_t = Target<tuple_element_t<Indexes, Tuple>...>;

    template<typename TupleA, typename IndexA, typename TupleB, typename IndexB, typename... Types>
    struct concat_two_tuples_helper : public concat_two_tuples_helper<TupleA, integral_constant<size_t, IndexA::value + 1u>,
                                                                      TupleB, integral_constant<size_t, IndexB::value>,
                                                                      Types..., tuple_element_t<IndexA::value, TupleA>> { };


    template<typename TupleA, typename TupleB, typename IndexB, typename... Types>
    struct concat_two_tuples_helper<TupleA, integral_constant<size_t, tuple_size<TupleA>::value>, TupleB, IndexB, Types...> : public
                          concat_two_tuples_helper<TupleA, integral_constant<size_t, tuple_size<TupleA>::value>,
                          TupleB, integral_constant<size_t, IndexB::value + 1u>,
                          Types..., tuple_element_t<IndexB::value, TupleB>> { };

    template<typename TupleA, typename TupleB, typename... Types>
    struct concat_two_tuples_helper<TupleA, integral_constant<size_t, tuple_size<TupleA>::value>, TupleB, integral_constant<size_t, tuple_size<TupleB>::value>, Types...>
    {
        using type = tuple<Types...>;
    };

    template<typename TupleA, typename TupleB>
    struct concat_two_tuples
    {
        using type = typename concat_two_tuples_helper<TupleA, integral_constant<size_t, 0u>, TupleB, integral_constant<size_t, 0u>>::type;
    };

    template<typename Result, typename Tuple, typename... Tuples>
    struct tuple_concat_helper : public tuple_concat_helper<typename concat_two_tuples<Result, Tuple>::type, Tuples...> { };

    template<typename Result, typename Tuple>
    struct tuple_concat_helper<Result, Tuple>
    {
        using type = typename concat_two_tuples<Result, Tuple>::type;
    };

    template<typename... Tuples>
    struct tuple_concat
    {
        using type = typename tuple_concat_helper<tuple<>, Tuples...>::type;
    };

    template<typename... Tuples>
    using tuple_concat_t = typename tuple_concat<Tuples...>::type;

    namespace internal
    {
        template<size_t Index = 0, class... Errors>
        std::string tupleToMessage(const std::tuple<Errors...>& t)
        {
            std::string name_str = std::get<Index>(t), value_str;
            if constexpr (is_string_type_v<std::tuple_element_t<Index + 1, std::tuple<Errors...>>>)
                value_str = std::get<Index + 1>(t);
            else
                value_str = std::to_string(std::get<Index + 1>(t));
            if constexpr ((Index + 2) >= sizeof...(Errors))
                return name_str + " = " + value_str + "\n";
            else
                return name_str + " = " + value_str + "\n" + tupleToMessage<Index + 2, Errors...>(t);
        }
    }

    template<class Exception = std::runtime_error, class... Errors>
    void err(string_view what, Errors&&... p)
    {
        if constexpr (sizeof...(Errors) % 2)
        {
            static_assert(is_string_type_v<decltype(std::get<0>(std::tuple<Errors...>(p...)))>, "Second parameter should be function name");
            std::tuple<Errors...> t(std::forward<Errors>(p)...);
            if constexpr (sizeof...(Errors) > 1)
                throw Exception(string(what) + "\n" + string("in function: ") + string(get<0>(t)) + "\n" + internal::tupleToMessage<1, Errors...>(t));
            else
                throw Exception(string(what) + "\n" + string(" in function: ") + string(get<0>(t)) + "\n");
        }
        else
        {
            if constexpr (sizeof...(Errors))
                throw Exception(string(what) + "\n" + internal::tupleToMessage<0, Errors...>(std::tuple<Errors...>(std::forward<Errors>(p)...)));
            else
                throw Exception(string(what) + "\n");
        }
    }

    template<class Tp, bool = std::is_default_constructible_v<std::decay_t<Tp>>>
    class checkable
    {
    protected:

        using value_type = std::decay_t<Tp>;
        using reference = value_type&;
        using const_reference = const value_type&;
        using rvalue_reference = value_type&&;

        value_type m_val;
        bool m_flag = false;

    public:

        checkable() = default;

        template<class Tp_>
        checkable(Tp_&& value) : m_val(std::forward<Tp_>(value)), m_flag(true) {}

        template<class Tp_>
        const_reference operator=(Tp_&& value)
        {
            m_val = std::forward<Tp_>(value);
            m_flag = true;
        }

        operator reference() noexcept { return m_val; }

        operator const_reference() const noexcept { return m_val; }

        reference get() noexcept { return m_val; }

        const_reference get() const noexcept { return m_val; }

        bool check() const noexcept { return m_flag; }

        void set() noexcept { m_flag = true; }

        void unset() noexcept { m_flag = false; }
    };

    template<class Tp>
    class checkable<Tp, false> : public checkable<Tp, true>
    {
    public:
        template<class Tp_>
        checkable(Tp_&& value) : checkable<Tp, true>(value) {}
    };

    template<class Tp>
    checkable(Tp&&) -> checkable<std::decay_t<Tp>>;

    template<class InputIterator, class Function>
    void for_each_fast(InputIterator b, InputIterator e, Function f)
    {
        typedef decltype(std::async(std::launch::async, std::for_each<InputIterator, Function>,
        std::declval<InputIterator>(), std::declval<InputIterator>(),std::declval<Function>())) future_type;

        const auto hc = thread::hardware_concurrency();
        const auto dst = distance(b, e);
        const auto per_thr = dst / hc;
        const auto mod = dst % hc;

        auto last = b;
        vector<future_type> handle;
        for (unsigned i = 0; i < hc - 1; ++i)
        {
            advance(last, per_thr + ((mod > i) ? 1 : 0));
            handle.push_back(async(std::launch::async, for_each<InputIterator, Function>, b, last, f));
            b = last;
        }
        for_each<InputIterator, Function>(b, e, f);
        for (auto& h : handle)
            h.wait();
    }

    template<class StringType>
    vector<StringType> split_string(const StringType& s, const unordered_set<typename StringType::value_type>& sep_seq)
    {
        vector<StringType> result;
        StringType cur;
        bool first = true;
        for (const auto& c : s)
        {
            if (first)
            {
                if (sep_seq.find(c) != sep_seq.end())
                    continue;
                first = false;
                cur.push_back(c);
                continue;
            }
            if (sep_seq.find(c) != sep_seq.end())
            {
                result.push_back(std::move(cur));
                first = true;
            }
            else
                cur.push_back(c);
        }
        if (cur.size())
            result.push_back(std::move(cur));
        return result;
    }
}
