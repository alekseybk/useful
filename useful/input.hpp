/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "utils.hpp"

namespace uf
{
    namespace input
    {

    }
    // namespace input

    inline namespace operators
    {
        inline namespace input_operators
        {
            template<typename... StreamArgs, typename... Ts>
            std::basic_istream<StreamArgs...>& operator>>(std::basic_istream<StreamArgs...>& stream, tuple<Ts...>& t)
            {
                utils::tuple_for_each(t, [&stream](auto& e){ stream >> e; });
                return stream;
            }

            template<typename... StreamArgs, typename F, typename S>
            std::basic_istream<StreamArgs...>& operator>>(std::basic_istream<StreamArgs...>& stream, pair<F, S>& p)
            {
                return stream >> p.first >> p.second;
            }
        }
        // namespace input_operators
    }
    // namespace operators
}
// namespace uf
