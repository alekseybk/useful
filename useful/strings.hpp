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
        auto build_split_tuple(IterVector&& iters, sequence<Ns...>)
        {
            return std::tuple(SeqContainer(iters[Ns].first, iters[Ns].second)...);
        }
    }
    // namespace detail

    inline namespace strings
    {
        constexpr bool is_digit(char c) noexcept
        {
            return c >= '0' && c <= '9';
        }

        constexpr bool is_lower(char c) noexcept
        {
            return c >= 'a' && c <= 'z';
        }

        constexpr bool is_upper(char c) noexcept
        {
            return c >= 'A' && c <= 'Z';
        }

        constexpr bool is_letter(char c) noexcept
        {
            return is_lower(c) || is_upper(c);
        }

        void lowercase(std::string& s)
        {
            for (char& c : s)
                if (is_upper(c))
                    c += 'a' - 'A';
        }

        void uppercase(std::string& s)
        {
            for (char& c : s)
                if (is_lower(c))
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
            return detail::build_split_tuple<SeqContainer>(split_itr(c, ds...), make_sequence<N>());
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
            return detail::build_split_tuple<SeqContainer>(split_strong_itr(c, ds...), make_sequence<N>());
        }

        template<class C1, class C2, disif<std::is_convertible_v<C2, typename C1::value_type>> = sdef>
        bool starts_with(const C1& c, const C2& pattern)
        {
            auto i1 = std::begin(c);
            auto i2 = std::begin(pattern);
            for (; i1 != std::end(c) && i2 != std::end(pattern); ++i1, ++i2)
                if (*i1 != *i2)
                    return false;
            return i2 == std::end(pattern) ? true : false;
        }

        template<class C, class E, enif<std::is_convertible_v<E, typename C::value_type>> = sdef>
        bool starts_with(const C& c, const E& e)
        {
            return !c.empty() && *std::begin(c) == e;
        }

        template<class C1, class C2, disif<std::is_convertible_v<C2, typename C1::value_type>> = sdef>
        bool ends_with(const C1& c, const C2& pattern)
        {
            auto i1 = std::rbegin(c);
            auto i2 = std::rbegin(pattern);
            for (; i1 != std::rend(c) && i2 != std::rend(pattern); ++i1, ++i2)
                if (*i1 != *i2)
                    return false;
            return i2 == std::rend(pattern) ? true : false;
        }

        template<class C, class E, enif<std::is_convertible_v<E, typename C::value_type>> = sdef>
        bool ends_with(const C& c, const E& e)
        {
            return !c.empty() && *std::rbegin(c) == e;
        }

        template<class C, u64 N>
        bool starts_with(const C& c, const char(&literal)[N])
        {
            return starts_with(c, std::string_view(literal, N - 1));
        }

        template<class C, u64 N>
        bool ends_with(const C& c, const char(&literal)[N])
        {
            return ends_with(c, std::string_view(literal, N - 1));
        }
    }
    // inline namespace strings
}
// namespace uf
