/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "meta.hpp"

namespace uf
{
    namespace utils
    {
        namespace detail
        {
            template<bool Minimum, typename Result, typename... Ts, meta::disable_if_t<meta::is_same_all_v<decay_t<Ts>...>, int> = 0>
            constexpr auto mm_helper(Ts&&... args)
            {
                static_assert(is_same_v<Result, decay_t<Result>>);

                // TODO: find same types, call mm_ref_helper
                Result m(*std::get<0>(tuple<remove_reference_t<Ts>*...>{&args...}));
                auto compare_function = [&m](auto&& another)
                {
                    if constexpr (Minimum)
                    {
                        if (another < m)
                            m = std::forward(another);
                    }
                    else
                    {
                        if (another > m)
                            m = std::forward(another);
                    }
                };
                (compare_function(std::forward<Ts>(args)), ...);
                return m;
            }

            template<bool Minimum, typename Result, typename... Ts, enable_if_t<meta::is_same_all_v<decay_t<Ts>...>, int> = 0>
            constexpr auto mm_helper(Ts&&... args)
            {
                static_assert(is_same_v<Result, decay_t<Result>>);
                using type = decay_t<meta::first_t<Ts...>>;
                const type* m = std::get<0>(tuple<remove_reference_t<Ts>*...>{&args...});
                if constexpr (Minimum)
                    ((m = (args < *m) ? &args : m), ...);
                else
                    ((m = (args > *m) ? &args : m), ...);
                return Result(*m);
            }

            template<bool Minimum, typename... Ts>
            constexpr auto& mm_ref_helper(Ts&&... args)
            {
                static_assert((is_lvalue_reference_v<Ts> && ...) && meta::is_same_all_v<decay_t<Ts>...>, "All arguments must be an lvalues of the same type");
                using type = remove_reference_t<meta::first_t<Ts...>>;
                using result_type = conditional_t<(is_const_v<remove_reference_t<Ts>> || ...), const type, type>;
                result_type* m = std::get<0>(tuple<remove_reference_t<Ts>*...>{&args...});
                if constexpr (Minimum)
                    ((m = (args < *m) ? &args : m), ...);
                else
                    ((m = (args > *m) ? &args : m), ...);
                return *m;
            }

            template<bool Result, typename Tuple, typename F, u64... Ns>
            auto for_each_tpl_helper(Tuple&& t, F&& f, index_sequence<Ns...>)
            {
                if constexpr (Result)
                    return tuple{f(std::get<Ns>(t))...};
                else
                    (f(std::get<Ns>(t)), ...);
            }
        }
        // namespace detail

        template<typename Owner, typename E>
        constexpr auto&& forward_element(E&& e)
        {
            using type = remove_reference_t<E>;

            if constexpr (!is_lvalue_reference_v<E>)
                return static_cast<type&&>(e);
            else if constexpr (is_lvalue_reference_v<Owner>)
                return static_cast<type&>(e);
            else
                return static_cast<type&&>(e);
        }

        template<bool Result = false, typename Tuple, typename F>
        auto for_each_tpl(Tuple&& t, F&& f)
        {
            using tuple_type = remove_reference_t<Tuple>;

            if constexpr (Result)
                return detail::for_each_tpl_helper<Result>(t, f, make_index_sequence<tuple_size_v<tuple_type>>());
            else
                detail::for_each_tpl_helper<Result>(t, f, make_index_sequence<tuple_size_v<tuple_type>>());
        }

        template<typename TargetType, typename Tp, typename F>
        void for_each_target(Tp&& container, const F& f)
        {
            using value_type = decay_t<typename remove_reference_t<Tp>::value_type>;

            if constexpr (is_same_v<decay_t<TargetType>, value_type>)
                std::for_each(container.begin(), container.end(), f);
            else
                for (auto& value : container)
                    for_each_target<TargetType>(forward_element<Tp>(value), f);
        }

        template<typename Result, typename... Ts>
        constexpr auto min(Ts&&... args)
        {
            return detail::mm_helper<true, Result>(std::forward<Ts>(args)...);
        }

        template<typename... Ts>
        constexpr auto min(Ts&&... args)
        {
            return detail::mm_helper<true, decay_t<meta::first_t<Ts...>>>(std::forward<Ts>(args)...);
        }

        template<typename... Ts>
        constexpr auto& min_ref(Ts&&... args)
        {
            return detail::mm_ref_helper<true, Ts...>(std::forward<Ts>(args)...);
        }

        template<typename Result, typename... Ts>
        constexpr auto max(Ts&&... args)
        {
            return detail::mm_helper<false, Result>(std::forward<Ts>(args)...);
        }

        template<typename... Ts>
        constexpr auto max(Ts&&... args)
        {
            return detail::mm_helper<false, decay_t<meta::first_t<Ts...>>>(std::forward<Ts>(args)...);
        }

        template<typename... Ts>
        constexpr auto& max_ref(Ts&&... args)
        {
            return detail::mm_ref_helper<false, Ts...>(std::forward<Ts>(args)...);
        }
    }
    // namespace utils
}
// namespace uf
