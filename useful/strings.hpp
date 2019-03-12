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
        template<class SeqContainer, class IterVector>
        auto build_split_vector(IterVector&& iters)
        {
            std::vector<SeqContainer> result;
            result.reserve(iters.size());
            for (auto [b, e] : iters)
                result.emplace_back(b, e);
            return result;
        }

        template<class SeqContainer, class IterVector, u64... Ns>
        auto build_split_tuple(IterVector&& iters, std::index_sequence<Ns...>)
        {
            return std::tuple(SeqContainer(iters[Ns].first, iters[Ns].second)...);
        }
    }
    // namespace detail

    void lowercase(std::string& s)
    {
        for (char& c : s)
            if (c >= 'A' && c <= 'Z')
                c += 'a' - 'A';
    }

    void uppercase(std::string& s)
    {
        for (char& c : s)
            if (c >= 'a' && c <= 'z')
                c -= 'a' - 'A';
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
        std::vector<std::pair<iter, iter>> result;
        for (auto i = c.begin(); i != c.end(); ++i)
        {
            if (satisfies_one(*i, ds...))
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
        return split_itr(std::numeric_limits<u64>::max(), c, ds...);
    }

    template<class SeqContainer, class... Ds>
    auto split(u64 n, const SeqContainer& c, Ds&&... ds)
    {
        return detail::build_split_vector<SeqContainer>(split_itr(n, c, ds...));
    }

    template<class SeqContainer, class... Ds>
    auto split(const SeqContainer& c, Ds&&... ds)
    {
        return split(std::numeric_limits<u64>::max(), c, ds...);
    }

    template<u64 N, class SeqContainer, class... Ds>
    auto split(const SeqContainer& c, Ds&&... ds)
    {
        return detail::build_split_tuple<SeqContainer>(split_itr(c, ds...), std::make_index_sequence<N>());
    }

    template<class SeqContainer, class... Ds>
    auto split_strong_itr(u64 n, const SeqContainer& c, Ds&&... ds)
    {
        using iter = typename SeqContainer::const_iterator;

        std::vector<std::pair<iter, iter>> result;
        iter next_item_begin = c.begin();
        for (auto i = c.begin(); i != c.end(); ++i)
        {
            if (satisfies_one(*i, ds...))
            {
                result.push_back({next_item_begin, i});
                if (result.size() == n)
                    return result;
                next_item_begin = i + 1;
            }
        }
        result.push_back({next_item_begin, c.end()});
        return result;
    }

    template<class SeqContainer, class... Ds>
    auto split_strong_itr(const SeqContainer& c, Ds&&... ds)
    {
        return split_strong_itr(std::numeric_limits<u64>::max(), c, ds...);
    }

    template<class SeqContainer, class... Ds>
    auto split_strong(u64 n, const SeqContainer& c, Ds&&... ds)
    {
        return detail::build_split_vector<SeqContainer>(split_strong_itr(n, c, ds...));
    }

    template<class SeqContainer, class... Ds>
    auto split_strong(const SeqContainer& c, Ds&&... ds)
    {
        return split_strong(std::numeric_limits<u64>::max(), c, ds...);
    }

    template<u64 N, class SeqContainer, class... Ds>
    auto split_strong(const SeqContainer& c, Ds&&... ds)
    {
        return detail::build_split_tuple<SeqContainer>(split_strong_itr(c, ds...), std::make_index_sequence<N>());
    }

    template<class SeqContainer1, class SeqContainer2, enif<mt::is_iterable_v<std::decay_t<SeqContainer2>>> = sdef>
    bool starts_with(const SeqContainer1& c, const SeqContainer2& pattern)
    {
        if (pattern.size() > c.size())
            return false;
        for (auto i1 = c.begin(), i2 = pattern.begin(); i2 != pattern.end(); ++i1, ++i2)
            if (*i1 != *i2)
                return false;
        return true;
    }

    template<class SeqContainer1, class First, disif<mt::is_iterable_v<std::decay_t<First>>> = sdef>
    bool starts_with(const SeqContainer1& c, const First& first)
    {
        return !c.empty() && *c.begin() == first;
    }

    template<class SeqContainer1, class SeqContainer2, enif<mt::is_iterable_v<std::decay_t<SeqContainer2>>> = sdef>
    bool ends_with(const SeqContainer1& c, const SeqContainer2& pattern)
    {
        if (pattern.size() > c.size())
            return false;
        for (auto i1 = c.rbegin(), i2 = pattern.rbegin(); i2 != pattern.rend(); ++i1, ++i2)
            if (*i1 != *i2)
                return false;
        return true;
    }

    template<class SeqContainer1, class First, disif<mt::is_iterable_v<std::decay_t<First>>> = sdef>
    bool ends_with(const SeqContainer1& c, const First& first)
    {
        return !c.empty() && *c.rbegin() == first;
    }

    template<typename Tp, enif<std::is_floating_point_v<Tp>> = sdef>
    Tp from_string(const std::string& s)
    {
        return std::stold(s);
    }

    template<typename Tp, enif<std::is_integral_v<Tp> && std::is_signed_v<Tp>> = sdef>
    Tp from_string(const std::string& s)
    {
        return std::stoll(s);
    }

    template<typename Tp, enif<std::is_integral_v<Tp> && std::is_unsigned_v<Tp>> = sdef>
    Tp from_string(const std::string& s)
    {
        return std::stoull(s);
    }

    template<typename Tp, typename Str, enif<std::is_same_v<Tp, std::string>> = sdef>
    Tp from_string(Str&& s)
    {
        return std::forward<Str>(s);
    }

    template<typename Tp>
    void from_string(const std::string& s, Tp& v)
    {
        v = from_string<Tp>(s);
    }
}
// namespace uf














