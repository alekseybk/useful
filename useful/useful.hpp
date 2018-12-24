/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "utils.hpp"

namespace uf
{
    namespace detail
    {
        template<class Container, class Bounds>
        auto create_split_vector(Bounds&& bounds)
        {
            vector<Container> result;
            result.reserve(bounds.size());
            for (const auto& [b, e] : bounds)
                result.emplace_back(b, e);
            return result;
        }

        template<class SplitVector, u64... Indexes>
        auto create_split_tuple(SplitVector&& vec, index_sequence<Indexes...>)
        {
            using elem_t = typename remove_reference_t<SplitVector>::value_type;
            return meta::tuple_same_n_t<elem_t, sizeof...(Indexes)>{utils::forward_element<SplitVector>(vec[Indexes])...};
        }

        template<class Container, class Delimiter, class... Delimiters>
        bool split_point_helper(const Container& c, typename Container::const_iterator i, Delimiter&& d, Delimiters&&... delimiters)
        {
            bool result;
            if constexpr (is_invocable_r_v<bool, Delimiter, const typename Container::value_type&>)
                result = d(*i);
            else if constexpr (is_invocable_r_v<bool, Delimiter, const Container&, typename Container::const_iterator>)
                result = d(c, i);
            else
                result = (*i == d);
            if (result)
                return true;
            if constexpr (sizeof...(Delimiters))
                return split_point_helper(c, i, delimiters...);
            else
                return false;
        }
    }
    // end namespace detail

    template<class Container, class... Delimiters>
    auto split(const Container& container, Delimiters&&... delimiters)
    {
        using container_t = remove_reference_t<Container>;
        using iterator_t = typename container_t::const_iterator;

        auto split_point = [&container, &delimiters...](iterator_t i) -> bool
        {
            return detail::split_point_helper(container, i, delimiters...);
        };

        bool empty_seq = true;
        vector<pair<iterator_t, iterator_t>> bounds;
        for (auto i = container.cbegin(); i != container.cend(); ++i)
        {
            if (empty_seq)
            {
                if (split_point(i))
                    continue;
                empty_seq = false;
                bounds.push_back({i, container.cend()});
                continue;
            }
            if (split_point(i))
            {
                bounds.back().second = i;
                empty_seq = true;
            }
        }
        return detail::create_split_vector<container_t>(std::move(bounds));
    }

    /// return N-sized tuple instead of vector
    template<u64 N, class Container, class... Delimiters>
    auto split(const Container& container, Delimiters&&... delimiters)
    {
        auto temp = split(std::forward<Container>(container), std::forward<Delimiters>(delimiters)...);
        return detail::create_split_tuple(std::move(temp), make_index_sequence<N>());
    }

    template<class Str>
    Str lowercase(const Str& s)
    {
        constexpr u64 dist = 'a' - 'A';
        Str result;
        result.reserve(s.size());
        for (auto c : s)
        {
            if (c >= 'A' && c <= 'Z')
                c += dist;
            result.push_back(c);
        }
        return result;
    }

    template<class Str>
    Str uppercase(const Str& s)
    {
        constexpr u64 dist = 'a' - 'A';
        Str result;
        result.reserve(s.size());
        for (auto c : s)
        {
            if (c >= 'a' && c <= 'z')
                c -= dist;
            result.push_back(c);
        }
        return result;
    }

    template<class Container, typename... Elems>
    Container strip(const Container& container, const Elems&... elems)
    {
        Container result;
        for (const auto& e : container)
            if (((e != elems) && ...))
                result.insert(result.end(), e);
        return result;
    }

    template<class Container, typename... Elems>
    Container strip_sides(const Container& container, const Elems&... elems)
    {
        auto fwd = container.cbegin();
        while (fwd != container.cend() && ((*fwd == elems) && ...))
            ++fwd;
        auto bck = container.crbegin();
        while (bck != container.crend() && ((*bck == elems) && ...))
            ++bck;
        return Container(fwd, bck.base());
    }

    template<class Stream>
    vector<vector<string>> read_csv(Stream&& stream, char delimiter = ',')
    {
        vector<vector<string>> result;

        string record;
        vector<string> line;
        bool qstate = false;

        string s;
        while (std::getline(stream, s))
        {
            for (u64 i = 0; i < s.size(); ++i)
            {
                if (qstate)
                {
                    if (s[i] == '\"')
                    {
                        if (i != s.size() - 1 && s[i + 1] == '\"')
                            ++i;
                        else
                        {
                            qstate = false;
                            continue;
                        }
                    }
                }
                else
                {
                    if (s[i] == delimiter)
                    {
                        line.push_back(std::move(record));
                        continue;
                    }
                    if (s[i] == '\"')
                    {
                        qstate = true;
                        continue;
                    }
                }
                record.push_back(s[i]);
            }
            if (!qstate)
            {
                line.push_back(std::move(record));
                result.push_back(std::move(line));
            }
        }
        return result;
    }

    template<class Container>
    auto position_pairs(Container&& container)
    {
        using value_t = typename remove_reference_t<Container>::value_type;
        using result_t = typename meta::clean<remove_reference_t<Container>>::template type<pair<u64, value_t>>;

        u64 j = 0;
        result_t result;
        for (auto i = container.cbegin(); i != container.cend(); ++i, ++j)
            result.push_back({j, utils::forward_element<Container>(*i)});
        return result;
    }

    template<class Container, class Compare>
    auto sort_save_position(Container&& container, Compare&& comp)
    {
        auto transformed = position_pairs(std::forward<Container>(container));
        sort(transformed.begin(), transformed.end(), [&comp](const auto& a, const auto& b)
        {
            return comp(a.second, b.second);
        });
        return transformed;
    }

    template<class AssociativeContainer, class Remover>
    void remove_associative(AssociativeContainer& container, Remover&& rm)
    {
        using mapped_t = typename AssociativeContainer::mapped_type;

        auto check = [&rm](const mapped_t& value)
        {
            if constexpr (is_invocable_r_v<bool, remove_reference_t<Remover>, const mapped_t&>)
                return rm(value);
            else
                return value == rm;
        };

        for (auto i = container.begin(); i != container.end();)
        {
            if (check(i->second))
                i = container.erase(i);
            else
                ++i;
        }
    }

    class spinlock
    {
        atomic_flag flag_ = ATOMIC_FLAG_INIT;

    public:
        bool try_lock() { return !flag_.test_and_set(std::memory_order_acquire); }

        void lock() { while (flag_.test_and_set(std::memory_order_acquire)) std::this_thread::yield(); }

        void unlock() { flag_.clear(std::memory_order_release); }
    };

    template<class Tp, template<class> class Compare>
    class dereference_compare
    {
        const Compare<Tp> comp_;

    public:
        template<class Pointer>
        bool operator()(const Pointer& a, const Pointer& b) const
        {
            return comp_(*a, *b);
        }
    };
}
// end namespace uf
