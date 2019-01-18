/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "utils.hpp"

namespace uf
{
    namespace output
    {

    }
    // namespace output

    inline namespace operators
    {
        inline namespace output_operators
        {
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
                utils::tuple_for_each(t, [&stream](const auto& e){ stream << e << " "; });
                return stream << ")";
            }

            template<typename... StreamArgs, class F, class S>
            std::basic_ostream<StreamArgs...>& operator<<(std::basic_ostream<StreamArgs...>& stream, const pair<F, S>& p)
            {
                return stream << "( " << p.first << " " << p.second << " )";
            }
        }
        // namespace output_operators
    }
    // namespace operators
}
// namespace uf
