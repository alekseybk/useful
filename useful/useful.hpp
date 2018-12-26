/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "utils.hpp"
#include "in_overloads.hpp"
#include "out_overloads.hpp"

namespace uf
{
    template<class Element, class P, class... Ps>
    bool satisfies_one(const Element& e, const P& p, const Ps&... ps);

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

        template<bool Lowercase>
        string ascii_lower_upper_case_helper(const string& s)
        {
            constexpr u64 dist = 'a' - 'A';
            string result;
            result.reserve(s.size());
            for (auto c : s)
            {
                if constexpr (Lowercase)
                {
                    if (c >= 'A' && c <= 'Z')
                        c += dist;
                }
                else
                {
                    if (c >= 'a' && c <= 'z')
                        c -= dist;
                }
                result.push_back(c);
            }
            return result;
        }

        template<bool ByKey, class AssociativeContainer, class... Rs>
        void remove_associative_helper(AssociativeContainer& c, const Rs&... rms)
        {
            using value_type = typename AssociativeContainer::value_type;

            for (auto i = c.cbegin(); i != c.cend();)
            {
                bool result;
                if constexpr (meta::is_pair_v<value_type>)
                {
                    if constexpr (ByKey)
                        result = satisfies_one(i->first, rms...);
                    else
                        result = satisfies_one(i->second, rms...);
                }
                else
                    result = satisfies_one(*i, rms...);
                if (result)
                    i = c.erase(i);
                else
                    ++i;
            }
        }
    }
    // end namespace detail

    string ascii_lowercase(const string& s) { return detail::ascii_lower_upper_case_helper<true>(s); }

    string ascii_uppercase(const string& s) { return detail::ascii_lower_upper_case_helper<false>(s); }

    template<class Element, class P, class... Ps>
    bool satisfies_one(const Element& e, const P& p, const Ps&... ps)
    {
        bool result;
        if constexpr (is_invocable_v<remove_reference_t<P>, const Element&>)
            result = p(e);
        else
            result = (e == p);
        if (result)
            return true;
        if constexpr (sizeof...(Ps))
            return satisfies_one(e, ps...);
        else
            return false;
    }

    template<class Element, class P, class... Ps>
    bool satisfies_all(const Element& e, const Ps&... ps) { return (satisfies_one(e, ps) && ...); }

    template<class Container, class... Ds>
    auto split(const Container& c, const Ds&... ds)
    {
        using container_t = remove_reference_t<Container>;
        using const_iterator_t = typename container_t::const_iterator;

        bool empty_seq = true;
        vector<pair<const_iterator_t, const_iterator_t>> bounds;
        for (auto i = c.cbegin(); i != c.cend(); ++i)
        {
            if (empty_seq)
            {
                if (satisfies_one(*i, ds...))
                    continue;
                empty_seq = false;
                bounds.push_back({i, c.cend()});
                continue;
            }
            if (satisfies_one(*i, ds...))
            {
                bounds.back().second = i;
                empty_seq = true;
            }
        }
        return detail::create_split_vector<container_t>(std::move(bounds));
    }

    /// return N-sized tuple instead of vector
    template<u64 N, class Container, class... Ds>
    auto split(const Container& c, const Ds&... ds)
    {
        auto temp = split(c, ds...);
        return detail::create_split_tuple(std::move(temp), make_index_sequence<N>());
    }

    template<class Container, typename... Ps>
    Container strip(const Container& c, const Ps&... ps)
    {
        Container result;
        for (const auto& e : c)
            if (!satisfies_one(e, ps...))
                result.insert(result.end(), e);
        return result;
    }

    template<class Container, typename... Ps>
    Container strip_sides(const Container& c, const Ps&... ps)
    {
        auto fwd = c.cbegin();
        while (fwd != c.cend() && satisfies_one(*fwd, ps...))
            ++fwd;
        auto bck = c.crbegin();
        while (bck != c.crend() && satisfies_one(*bck, ps...))
            ++bck;
        return Container(fwd, bck.base());
    }

    template<class Stream>
    vector<vector<string>> read_csv(Stream&& stream, char delimiter = ',') noexcept(false)
    {
        u64 cols = 0;
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
                if (cols == 0)
                    cols = line.size();
                if (line.size() != cols)
                    throw runtime_error("invalid line "s + to_string(result.size()) +
                                        ", size = " + to_string(line.size()) + ", expected = " + to_string(cols));
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
    auto sort_save_position(Container&& container, const Compare& comp)
    {
        auto transformed = position_pairs(std::forward<Container>(container));
        sort(transformed.begin(), transformed.end(), [&comp](const auto& a, const auto& b)
        {
            return comp(a.second, b.second);
        });
        return transformed;
    }

    template<class AssociativeContainer, class... Rs>
    void remove_associative_by_key(AssociativeContainer& c, const Rs&... rms)
    { detail::remove_associative_helper<true>(c, rms...); }


    template<class AssociativeContainer, class... Rs>
    void remove_associative_by_value(AssociativeContainer& c, const Rs&... rms)
    { detail::remove_associative_helper<false>(c, rms...); }

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
