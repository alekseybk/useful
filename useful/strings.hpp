/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "default_import.hpp"

#include "utils.hpp"

namespace uf
{
    namespace detail
    {
        template<bool Lowercase, class String>
        void ascii_case_helper(String& s)
        {
            for (auto& c : s)
            {
                if constexpr (Lowercase)
                {
                    if (c >= 'A' && c <= 'Z')
                        c += 'a' - 'A';
                }
                else
                {
                    if (c >= 'a' && c <= 'z')
                        c -= 'a' - 'A';
                }
            }
        }

        template<class SeqContainer, class IterVector>
        auto create_split_vector(IterVector&& iters)
        {
            vector<SeqContainer> result;
            result.reserve(iters.size());
            for (auto [b, e] : iters)
                result.emplace_back(b, e);
            return result;
        }

        template<class SeqContainer, class IterVector, u64... Ns>
        auto create_split_tuple(IterVector&& iters, index_sequence<Ns...>)
        {
            return tuple(SeqContainer(iters[Ns].first, iters[Ns].second)...);
        }
    }
    // namespace detail

    template<class String>
    void ascii_lowercase(String& s)
    {
        detail::ascii_case_helper<true>(s);
    }

    template<class String>
    void ascii_uppercase(String& s)
    {
        detail::ascii_case_helper<false>(s);
    }

    template<class SeqContainer, typename... Ps>
    void lstrip(SeqContainer& c, Ps&&... ps)
    {
        auto fwd = c.begin();
        while (fwd != c.end() && satisfies_one(*fwd, ps...))
            ++fwd;
        c.erase(c.begin(), fwd);
    }

    template<class SeqContainer, typename... Ps>
    void rstrip(SeqContainer& c, Ps&&... ps)
    {
        auto bck = c.rbegin();
        while (bck != c.rend() && satisfies_one(*bck, ps...))
            ++bck;
        c.erase(bck.base(), c.end());
    }

    template<class SeqContainer, typename... Ps>
    void strip(SeqContainer& c, Ps&&... ps)
    {
        lstrip(c, ps...);
        rstrip(c, ps...);
    }

    template<class SeqContainer, class... Ds>
    auto split_itr(u64 n, const SeqContainer& c, Ds&&... ds)
    {
        using iter = typename SeqContainer::const_iterator;

        bool empty_seq = true;
        vector<pair<iter, iter>> result;
        for (auto i = c.begin(); i != c.end(); ++i)
        {
            if (utils::satisfies_one(*i, ds...))
            {
                if (empty_seq)
                    continue;
                result.back().second = i;
                empty_seq = true;
                if (result.size() == n)
                    break;
            }
            else
            {
                if (empty_seq)
                {
                    empty_seq = false;
                    result.push_back({i, c.end()});
                }
            }
        }
        return result;
    }

    template<class SeqContainer, class... Ds>
    auto split_itr(const SeqContainer& c, Ds&&... ds)
    {
        return split_itr(numeric_limits<u64>::max(), c, ds...);
    }

    template<class SeqContainer, class... Ds>
    auto split(u64 n, const SeqContainer& c, Ds&&... ds)
    {
        return detail::create_split_vector<SeqContainer>(split_itr(n, c, ds...));
    }

    template<class SeqContainer, class... Ds>
    auto split(const SeqContainer& c, Ds&&... ds)
    {
        return split(numeric_limits<u64>::max(), c, ds...);
    }

    template<u64 N, class SeqContainer, class... Ds>
    auto split(const SeqContainer& c, Ds&&... ds)
    {
        return detail::create_split_tuple<SeqContainer>(split_itr(c, ds...), make_index_sequence<N>());
    }

    template<class SeqContainer, class... Ds>
    auto split_strong_itr(u64 n, const SeqContainer& c, Ds&&... ds)
    {
        using iter = typename SeqContainer::const_iterator;

        vector<pair<iter, iter>> result;
        iter next_item_begin = c.begin();
        for (auto i = c.begin(); i != c.end(); ++i)
        {
            if (utils::satisfies_one(*i, ds...))
            {
                result.push_back({next_item_begin, i});
                next_item_begin = i + 1;
            }
        }
        result.push_back({next_item_begin, c.end()});
        return result;
    }

    template<class SeqContainer, class... Ds>
    auto split_strong_itr(const SeqContainer& c, Ds&&... ds)
    {
        return split_strong_itr(numeric_limits<u64>::max(), c, ds...);
    }

    template<class SeqContainer, class... Ds>
    auto split_strong(u64 n, const SeqContainer& c, Ds&&... ds)
    {
        return detail::create_split_vector<SeqContainer>(split_strong_itr(n, c, ds...));
    }

    template<class SeqContainer, class... Ds>
    auto split_strong(const SeqContainer& c, Ds&&... ds)
    {
        return split_strong(numeric_limits<u64>::max(), c, ds...);
    }

    template<u64 N, class SeqContainer, class... Ds>
    auto split_strong(const SeqContainer& c, Ds&&... ds)
    {
        return detail::create_split_tuple<SeqContainer>(split_strong_itr(c, ds...), make_index_sequence<N>());
    }

    template<class SeqContainer1, class SeqContainer2, enable_if_t<meta::is_iterable_v<decay_t<SeqContainer2>>, int> = 0>
    bool starts_with(const SeqContainer1& c, const SeqContainer2& pattern)
    {
        if (pattern.size() > c.size())
            return false;
        for (auto i1 = c.begin(), i2 = pattern.begin(); i2 != pattern.end(); ++i1, ++i2)
            if (*i1 != *i2)
                return false;
        return true;
    }

    template<class SeqContainer1, class First, disable_if_t<meta::is_iterable_v<decay_t<First>>, int> = 0>
    bool starts_with(const SeqContainer1& c, const First& first)
    {
        return !c.empty() && *c.begin() == first;
    }

    template<class SeqContainer1, class SeqContainer2, enable_if_t<meta::is_iterable_v<decay_t<SeqContainer2>>, int> = 0>
    bool ends_with(const SeqContainer1& c, const SeqContainer2& pattern)
    {
        if (pattern.size() > c.size())
            return false;
        for (auto i1 = c.rbegin(), i2 = pattern.rbegin(); i2 != pattern.rend(); ++i1, ++i2)
            if (*i1 != *i2)
                return false;
        return true;
    }

    template<class SeqContainer1, class First, disable_if_t<meta::is_iterable_v<decay_t<First>>, int> = 0>
    bool ends_with(const SeqContainer1& c, const First& first)
    {
        return !c.empty() && *c.rbegin() == first;
    }
}
// namespace uf














