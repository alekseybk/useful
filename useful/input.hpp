/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "default_import.hpp"

namespace uf
{
    namespace input
    {
        namespace detail
        {
            template<class Stream, typename... Ts, u64... Ns>
            void tuple_read_helper(Stream& stream, tuple<Ts...>& t, index_sequence<Ns...>)
            {
                (stream >> ... >> std::get<Ns>(t));
            }
        }
        // end namespace detail

        template<typename... StreamArgs, typename... Ts>
        std::basic_istream<StreamArgs...>& operator>>(std::basic_istream<StreamArgs...>& stream, tuple<Ts...>& t)
        {
            detail::tuple_read_helper(stream, t, make_index_sequence<sizeof...(Ts)>());
            return stream;
        }

        template<typename... StreamArgs, typename F, typename S>
        std::basic_istream<StreamArgs...>& operator>>(std::basic_istream<StreamArgs...>& stream, pair<F, S>& p)
        {
            return stream >> p.first >> p.second;
        }
    }
    // end namespace input
}
// end namespace uf
