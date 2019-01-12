/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "meta.hpp"

namespace uf
{
    namespace out_overloads
    {
        namespace detail
        {
            template<typename Stream, typename... Ts, u64... Ns>
            void print_tuple_helper(Stream& stream, const tuple<Ts...>& t, std::index_sequence<Ns...>)
            {
                ((stream << std::get<Ns>(t) << " "), ...);
            }
        }
        // end namespace detail

        template<typename... StreamArgs, class Container, enable_if_t<meta::is_iterable_v<Container>, int> = 0>
        std::basic_ostream<StreamArgs...>& operator<<(std::basic_ostream<StreamArgs...>& stream, const Container& c)
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

        template<typename... StreamArgs, class Tp>
        std::basic_ostream<StreamArgs...>& operator<<(std::basic_ostream<StreamArgs...>& stream, const vector<vector<Tp>>& m)
        {
            if (m.empty())
                return stream;
            stream << m[0];
            for (size_t i = 1; i < m.size(); ++i)
                stream << "\n" << m[i];
            return stream;
        }
    }
    // end namespace out_overloads
}
// end namespace uf
