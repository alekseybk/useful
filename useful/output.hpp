/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "meta.hpp"

namespace uf
{
    namespace output
    {
        namespace detail
        {
            template<typename Stream, typename... Ts, u64... Ns>
            void print_tuple_helper(Stream& stream, const tuple<Ts...>& t, std::index_sequence<Ns...>)
            {
                ((stream << std::get<Ns>(t) << " "), ...);
            }
        }
        // namespace detail

        template<typename... StreamArgs, class Iterable, enable_if_t<meta::is_iterable_v<Iterable>, int> = 0>
        std::basic_ostream<StreamArgs...>& operator<<(std::basic_ostream<StreamArgs...>& stream, const Iterable& c)
        {
            stream << "[ ";
            for (const auto& e : c)
                stream << e << " ";
            return stream << "]";
        }

        template<typename... StreamArgs, typename... Ts>
        std::basic_ostream<StreamArgs...>& operator<<(std::basic_ostream<StreamArgs...>& stream, const tuple<Ts...>& t)
        {
            stream << "( ";
            detail::print_tuple_helper(stream, t, make_index_sequence<sizeof...(Ts)>());
            return stream << ")";
        }

        template<typename... StreamArgs, class F, class S>
        std::basic_ostream<StreamArgs...>& operator<<(std::basic_ostream<StreamArgs...>& stream, const pair<F, S>& p)
        {
            return stream << "( " << p.first << " " << p.second << " )";
        }
    }
    // namespace output
}
// namespace uf
