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
        template<typename S>
        std::string lowercase(S&& s)
        {
            std::string result(std::forward<S>(s));
            std::for_each(result.begin(), result.end(), [](auto& c){ c = std::tolower(c); });
            return result;
        }

        template<typename S>
        std::string uppercase(S&& s)
        {
            std::string result(std::forward<S>(s));
            std::for_each(result.begin(), result.end(), [](auto& c){ c = std::toupper(c); });
            return result;
        }

        template<class SeqContainer, typename... Ps>
        void lstrip(SeqContainer& c, Ps&&... ps)
        {
            auto fwd = c.begin();
            while (fwd != c.end() && std_any(*fwd, ps...))
                ++fwd;
            c.erase(c.begin(), fwd);
        }

        template<class SeqContainer, typename... Ps>
        void rstrip(SeqContainer& c, Ps&&... ps)
        {
            auto bck = c.rbegin();
            while (bck != c.rend() && std_any(*bck, ps...))
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

            iter next_begin = c.begin();
            std::vector<std::pair<iter, iter>> result;
            if (!n)
                return result;
            for (auto i = c.begin(); i != c.end(); ++i)
            {
                if (stf_any(*i, ds...))
                {
                    if (i == next_begin)
                    {
                        ++next_begin;
                        continue;
                    }
                    result.push_back({next_begin, i});
                    next_begin = std::next(i);
                    if (result.size() == n)
                        return result;
                }
            }
            if (next_begin != c.end())
                result.push_back({next_begin, c.end()});
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

            iter next_begin = c.begin();
            std::vector<std::pair<iter, iter>> result;
            if (!n)
                return result;
            for (auto i = c.begin(); i != c.end(); ++i)
            {
                if (stf_any(*i, ds...))
                {
                    result.push_back({next_begin, i});
                    next_begin = std::next(i);
                    if (result.size() == n)
                        return result;
                }
            }
            result.push_back({next_begin, c.end()});
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
