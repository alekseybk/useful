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
            template<bool Minimum, typename Result, typename... Ts, enable_if_t<!meta::is_same_all_v<decay_t<Ts>...>, int> = 0>
            constexpr auto min_max_1(Ts&&... args)
            {
                static_assert(is_same_v<Result, decay_t<Result>>);

                Result m;
                m = *std::get<0>(tuple<remove_reference_t<Ts>*...>{&args...});
                if constexpr (Minimum)
                    ((m = (args < m) ? std::forward<Ts>(args) : std::move(m)), ...);
                else
                    ((m = (args > m) ? std::forward<Ts>(args) : std::move(m)), ...);
                return m;
            }

            template<bool Minimum, typename Result, typename... Ts, enable_if_t<meta::is_same_all_v<decay_t<Ts>...>, int> = 0>
            constexpr auto min_max_1(Ts&&... args)
            {
                static_assert(is_same_v<Result, decay_t<Result>>);
                using type = decay_t<meta::first_t<Ts...>>;

                const type* m = std::get<0>(tuple<remove_reference_t<Ts>*...>{&args...});
                if constexpr (Minimum)
                    ((m = (args < *m) ? &args : m), ...);
                else
                    ((m = (args > *m) ? &args : m), ...);
                if constexpr (is_same_v<Result, type>)
                    return *m;
                else
                    return Result(*m);
            }

            template<bool Minimum, typename... Ts>
            constexpr auto& min_max_ref_1(Ts&&... args)
            {
                static_assert((is_lvalue_reference_v<Ts> && ...) && meta::is_same_all_v<decay_t<Ts>...>, "All arguments must be an lvalues of the same type");
                using type = conditional_t<(is_const_v<remove_reference_t<Ts>> || ...),
                             const remove_reference_t<meta::first_t<Ts...>>,
                             remove_reference_t<meta::first_t<Ts...>>>;

                reference_wrapper<type> m = *std::get<0>(tuple<remove_reference_t<Ts>*...>{&args...});
                if constexpr (Minimum)
                    ((m = (args < m.get()) ? reference_wrapper<type>(args) : m), ...);
                else
                    ((m = (args > m.get()) ? reference_wrapper<type>(args) : m), ...);
                return m.get();
            }
        }
        // end namespace detail

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
            return detail::min_max_1<true, Result>(std::forward<Ts>(args)...);
        }

        template<typename... Ts>
        constexpr auto min(Ts&&... args)
        {
            return detail::min_max_1<true, decay_t<meta::first_t<Ts...>>>(std::forward<Ts>(args)...);
        }

        template<typename... Ts>
        constexpr auto& min_ref(Ts&&... args)
        {
            return detail::min_max_ref_1<true, Ts...>(std::forward<Ts>(args)...);
        }

        template<typename Result, typename... Ts>
        constexpr auto max(Ts&&... args)
        {
            return detail::min_max_1<false, Result>(std::forward<Ts>(args)...);
        }

        template<typename... Ts>
        constexpr auto max(Ts&&... args)
        {
            return detail::min_max_1<false, decay_t<meta::first_t<Ts...>>>(std::forward<Ts>(args)...);
        }

        template<typename... Ts>
        constexpr auto& max_ref(Ts&&... args)
        {
            return detail::min_max_ref_1<false, Ts...>(std::forward<Ts>(args)...);
        }
    }
    // end namespace utils
}
// end namespace uf
