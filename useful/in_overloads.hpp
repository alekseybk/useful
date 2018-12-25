/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "meta.hpp"

namespace uf
{
    namespace in_overloads
    {
        namespace detail
        {
            template<class Stream, typename... Ts, u64... Ns>
            void tuple_read_helper(Stream& stream, tuple<Ts...>& t, index_sequence<Ns...>)
            {
                (stream >> ... >> std::get<Ns>(t));
            }

            template<class Stream, typename... Ts>
            void read(Stream& stream, tuple<Ts...>& t)
            {
                tuple_read_helper(stream, t, make_index_sequence<sizeof...(Ts)>());
            }

            template<class Stream, typename F, typename S>
            void read(Stream& stream, pair<F, S>& t)
            {
                stream >> t.first >> t.second;
            }
        }
        // end namespace detail

        template<typename Object, typename... StreamArgs>
        std::basic_istream<StreamArgs...>& operator>>(std::basic_istream<StreamArgs...>& stream, Object& object)
        {
            detail::read(stream, object);
            return stream;
        }
    }
    // end namespace in_overloads
}
// end namespace uf
