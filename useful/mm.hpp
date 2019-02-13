/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "meta.hpp"

namespace uf::utils
{
    namespace detail
    {
        /*
        TODO: all this shit

        template<bool Minimum, typename Result, typename... Ts>
        constexpr Result mm_unique_types_helper(Ts&&... args)
        {

        }
        */

        template<bool Minimum, typename Result, typename... Ts, meta::disable_if_t<meta::is_same_all_v<std::decay_t<Ts>...>, int> = 0>
        constexpr Result mm_helper(Ts&&... args)
        {
            static_assert(std::is_same_v<Result, std::decay_t<Result>>);

            Result m(*std::get<0>(std::tuple<std::remove_reference_t<Ts>*...>{&args...}));
            constexpr auto mm_update = [&m](auto&& another) -> void
            {
                // TODO: forward tmpl arg
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
            (mm_update(std::forward<Ts>(args)), ...);
            return m;
        }

        template<bool Minimum, u64 M, u64 Cur, typename Result, typename TupleOfRefs>
        constexpr Result mm_same_types_helper(TupleOfRefs&& t)
        {
            if constexpr (Cur == std::tuple_size_v<std::remove_reference_t<TupleOfRefs>>)
                return std::get<M>(std::move(t));
            else
            {
                if constexpr (Minimum)
                {
                    if (std::get<Cur>(t) < std::get<M>(t))
                        return mm_same_types_helper<Minimum, Cur, Cur + 1, Result>(t);
                }
                else
                {
                    if (std::get<Cur>(t) > std::get<M>(t))
                        return mm_same_types_helper<Minimum, Cur, Cur + 1, Result>(t);
                }
                return mm_same_types_helper<Minimum, M, Cur + 1, Result>(t);
            }
        }

        template<bool Minimum, typename Result, typename... Ts, std::enable_if_t<meta::is_same_all_v<std::decay_t<Ts>...>, int> = 0>
        constexpr Result mm_helper(Ts&&... args)
        {
            static_assert(std::is_same_v<Result, std::decay_t<Result>>);
            return mm_same_types_helper<Minimum, 0, 0, Result>(std::forward_as_tuple(std::forward<Ts>(args)...));
        }

        template<bool Minimum, typename First, typename... Ts>
        constexpr auto& mm_ref_helper(First& first, Ts&... args)
        {
            static_assert(std::is_same_v<std::decay_t<First>, std::decay_t<meta::first_t<Ts...>>>);
            using result_type = std::conditional_t<(std::is_const_v<Ts> || ...) || std::is_const_v<First>, const First, First>;

            result_type* m = &first;
            if constexpr (Minimum)
                ((m = (args < *m) ? &args : m), ...);
            else
                ((m = (args > *m) ? &args : m), ...);
            return *m;
        }
    }
    // namespace detail

    template<typename Result, typename... Ts>
    constexpr Result min(Ts&&... args)
    {
        return detail::mm_helper<true, Result>(std::forward<Ts>(args)...);
    }

    template<typename... Ts>
    constexpr auto min(Ts&&... args)
    {
        return detail::mm_helper<true, std::decay_t<meta::first_t<Ts...>>>(std::forward<Ts>(args)...);
    }

    template<typename... Ts>
    constexpr auto& rmin(Ts&... args)
    {
        return detail::mm_ref_helper<true, Ts...>(args...);
    }

    template<typename Result, typename... Ts>
    constexpr Result max(Ts&&... args)
    {
        return detail::mm_helper<false, Result>(std::forward<Ts>(args)...);
    }

    template<typename... Ts>
    constexpr auto max(Ts&&... args)
    {
        return detail::mm_helper<false, std::decay_t<meta::first_t<Ts...>>>(std::forward<Ts>(args)...);
    }

    template<typename... Ts>
    constexpr auto& rmax(Ts&... args)
    {
        return detail::mm_ref_helper<false, Ts...>(args...);
    }
}
// namespace uf::utils