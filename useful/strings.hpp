#pragma once
#include "utils.hpp"

namespace uf
{
    namespace detail
    {

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
            while (fwd != c.end() && stf_any(*fwd, ps...))
                ++fwd;
            c.erase(c.begin(), fwd);
        }

        template<class SeqContainer, typename... Ps>
        void rstrip(SeqContainer& c, Ps&&... ps)
        {
            auto bck = c.rbegin();
            while (bck != c.rend() && stf_any(*bck, ps...))
                ++bck;
            c.erase(bck.base(), c.end());
        }

        template<class SeqContainer, typename... Ps>
        void strip(SeqContainer& c, Ps&&... ps)
        {
            lstrip(c, ps...);
            rstrip(c, ps...);
        }

        template<class SeqContainer, typename... Ds>
        auto split_itr_n(const SeqContainer& c, u64 n, Ds&&... ds)
        {
            using iter = typename SeqContainer::const_iterator;

            std::vector<std::pair<iter, iter>> result;
            auto&& fobject = stf_any_obj(std::forward<Ds>(ds)...);
            const auto wrp = std::ref(fobject);
            for (iter next_begin = std::find_if_not(c.begin(), c.end(), wrp); n && next_begin != c.end();)
            {
                iter next_delimiter = std::find_if(std::next(next_begin), c.end(), wrp);
                result.push_back({next_begin, next_delimiter});
                if (next_delimiter == c.end())
                    break;
                next_begin = std::find_if_not(std::next(next_delimiter), c.end(), wrp);
                --n;
            }
            return result;
        }

        template<class SeqContainer, typename... Ds>
        auto split_itr(const SeqContainer& c, Ds&&... ds)
        {
            return split_itr_n(c, std::numeric_limits<u64>::max(), ds...);
        }

        template<class SeqContainer>
        auto build_split_vector(const std::vector<std::pair<typename SeqContainer::const_iterator, typename SeqContainer::const_iterator>>& bounds)
        {
            std::vector<SeqContainer> result;
            result.reserve(bounds.size());
            for (auto [b, e] : bounds)
                result.emplace_back(b, e);
            return result;
        }

        template<class SeqContainer, typename... Ds>
        auto split_n(const SeqContainer& c, u64 n, Ds&&... ds)
        {
            return build_split_vector<SeqContainer>(split_itr_n(c, n, ds...));
        }

        template<class SeqContainer, typename... Ds>
        auto split(const SeqContainer& c, Ds&&... ds)
        {
            return split_n(c, std::numeric_limits<u64>::max(), ds...);
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
