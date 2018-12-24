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
            template<class Stream, typename Tp, meta::disable_if_t<meta::has_begin_end_v<Tp>, int> = 0>
            void print(Stream& stream, const Tp& value) { stream << value; }

            template<class Stream, class Container, enable_if_t<meta::has_begin_end_v<Container>, int> = 0>
            void print(Stream& stream, const Container& container)
            {
                stream << "c" << container.size() << "[";
                auto i = container.cbegin();
                if (!container.empty())
                    stream << *i++;
                for (; i != container.cend(); ++i)
                    stream << ", " << *i;
                stream << "]";
            }

            template<typename Stream, typename... Ts, u64... Ns>
            void print_tuple_helper(Stream& stream, const tuple<Ts...>& t, std::index_sequence<Ns...>)
            {
                ((stream << ", " << std::get<Ns>(t)), ...);
            }

            template<class Stream, typename... Ts>
            void print(Stream& stream, const tuple<Ts...>& t)
            {
                stream << "t[";
                if constexpr (sizeof...(Ts))
                {
                    stream << std::get<0>(t);
                    print_tuple_helper(stream, t, meta::make_custom_index_sequence<1, sizeof...(Ts)>());
                }
                stream << "]";
            }

            template<typename Stream, typename F, typename S>
            void print(Stream& stream, const pair<F, S>& p)
            {
                stream << "p[" << p.first << ", " << p.second << "]";
            }
        }
        // end namespace detail

        template<typename Object, typename... StreamArgs>
        std::basic_ostream<StreamArgs...>& operator<<(std::basic_ostream<StreamArgs...>& stream, const Object& object)
        {
            detail::print(stream, object);
            return stream;
        }
    }
    // end namespace out_overloads
}
// end namespace uf
