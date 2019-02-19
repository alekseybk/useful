/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "meta.hpp"

namespace uf::ls
{
    template<class E, class... Ps>
    constexpr bool satisfies_one(const E& e, Ps&&... ps);

    namespace detail
    {
        template<typename E, typename P>
        bool satisfies_helper(const E& e, P&& p)
        {
            if constexpr (std::is_invocable_v<std::remove_reference_t<P>, const E&>)
                return std::invoke(std::forward<P>(p), e);
            else
                return e == std::forward<P>(p);
        }

        template<typename Tuple, typename F, u64... Ns>
        constexpr void tuple_for_each_helper(Tuple&& t, F&& f, std::index_sequence<Ns...>)
        {
            (std::invoke(std::forward<F>(f), std::get<Ns>(std::forward<Tuple>(t))), ...);
        }

        template<bool ByKey, typename E, class... Rs>
        constexpr bool remove_assoc_element_check(const E& e, Rs&&... rs)
        {
            if constexpr (mt::is_pair_v<E>)
            {
                if constexpr (ByKey)
                    return satisfies_one(e.first, rs...);
                else
                    return satisfies_one(e.second, rs...);
            }
            else
                return satisfies_one(e, rs...);
        }

        template<bool ByKey, class AssocContainer, class... Rs>
        void remove_assoc_helper(AssocContainer& c, Rs&&... rs)
        {
            for (auto i = c.cbegin(); i != c.cend();)
            {
                if (detail::remove_assoc_element_check<ByKey>(*i, rs...))
                    i = c.erase(i);
                else
                    ++i;
            }
        }

        template<bool ByKey, class AssocContainer, class... Rs>
        AssocContainer remove_assoc_copy_helper(const AssocContainer& c, Rs&&... rs)
        {
            AssocContainer result;
            for (auto i = c.cbegin(); i != c.cend(); ++i)
                if (!detail::remove_assoc_element_check<ByKey>(*i, rs...))
                    result.insert(*i);
            return result;
        }

        template<typename Index, typename Tp, class F>
        std::pair<bool, Index> binary_search_impl(Index left, Index right, Tp&& value, F&& f)
        {
            const Index middle = left + (right - left) / 2;
            if (right - middle <= 1)
            {
                if (f(right) == value)
                    return {false, right};
                if (f(left) == value)
                    return {false, left};
                return {true, left};
            }
            if (f(middle) < value)
                return binary_search_impl(middle, right, std::forward<Tp>(value), std::forward<F>(f));
            return binary_search_impl(left, middle, std::forward<Tp>(value), std::forward<F>(f));
        }
    }
    // namespace detail

    template<typename Owner, typename E, std::enable_if_t<std::is_lvalue_reference_v<Owner>, int> = 0>
    constexpr auto&& forward_element(E&& e)
    {
        return std::forward<E>(e);
    }

    template<typename Owner, typename E, std::enable_if_t<std::is_rvalue_reference_v<Owner> || !std::is_reference_v<Owner>, int> = 0>
    constexpr auto&& forward_element(E&& e)
    {
        return std::move(e);
    }

    template<class SeqContainer>
    auto add_positions(SeqContainer&& container)
    {
        using value_type = typename std::remove_reference_t<SeqContainer>::value_type;
        using result_type = typename mt::clean<std::remove_reference_t<SeqContainer>>::template type<std::pair<u64, value_type>>;

        u64 j = 0;
        result_type result;
        for (auto i = container.begin(); i != container.end(); ++i, ++j)
            result.push_back({j, utils::forward_element<SeqContainer>(*i)});
        return result;
    }

    template<class SeqContainer, class Compare>
    auto sort_save_position(SeqContainer&& container, const Compare& comp)
    {
        auto pairs = add_positions(std::forward<SeqContainer>(container));
        sort(pairs.begin(), pairs.end(), [&comp](const auto& a, const auto& b)
        {
            return comp(a.second, b.second);
        });
        return pairs;
    }

    template<class E, class... Ps>
    constexpr bool satisfies_one(const E& e, Ps&&... ps)
    {
        return (detail::satisfies_helper(e, std::forward<Ps>(ps)) || ...);
    }

    template<class E, class... Ps>
    bool satisfies_all(const E& e, Ps&&... ps)
    {
        return (detail::satisfies_helper(e, std::forward<Ps>(ps)) && ...);
    }

    template<u64... Ns, typename Tuple, typename F, typename = std::enable_if_t<(sizeof...(Ns) > 1)>>
    constexpr void tuple_for_each(Tuple&& t, F&& f)
    {
        detail::tuple_for_each_helper(std::forward<Tuple>(t), std::forward<F>(f), std::index_sequence<Ns...>());
    }

    template<u64 N, typename Tuple, typename F>
    constexpr void tuple_for_each(Tuple&& t, F&& f)
    {
        detail::tuple_for_each_helper(std::forward<Tuple>(t), std::forward<F>(f), std::make_index_sequence<N>());
    }

    template<typename Tuple, typename F>
    constexpr void tuple_for_each(Tuple&& t, F&& f)
    {
        tuple_for_each<std::tuple_size_v<std::remove_reference_t<Tuple>>>(std::forward<Tuple>(t), std::forward<F>(f));
    }

    template<typename Target, typename Container, typename F>
    void targeted_for_each(Container&& c, F&& f)
    {
        using value_type = std::decay_t<typename std::remove_reference_t<Container>::value_type>;

        if constexpr (std::is_same_v<std::decay_t<Target>, value_type>)
            std::for_each(c.begin(), c.end(), f);
        else
            for (auto& value : c)
                targeted_for_each<Target>(forward_element<Container>(value), f);
    }

    template<class AssocContainer, class... Rs>
    void remove_assoc_key(AssocContainer& c, Rs&&... rs)
    {
        detail::remove_assoc_helper<true>(c, rs...);
    }

    template<class AssocContainer, class... Rs>
    void remove_assoc_value(AssocContainer& c, Rs&&... rs)
    {
        detail::remove_assoc_helper<false>(c, rs...);
    }

    template<class AssocContainer, class... Rs>
    AssocContainer remove_assoc_copy_key(const AssocContainer& c, Rs&&... rs)
    {
        return detail::remove_assoc_copy_helper<true>(c, rs...);
    }

    template<class AssocContainer, class... Rs>
    AssocContainer remove_assoc_copy_value(const AssocContainer& c, Rs&&... rs)
    {
        return detail::remove_assoc_copy_helper<false>(c, rs...);
    }

    template<class SeqContainer, typename... Rs>
    void remove_seq(SeqContainer& c, Rs&&... rs)
    {
        auto cur_end = c.begin();
        for (auto& e : c)
            if (!satisfies_one(e, rs...))
                *cur_end++ = std::move(e);
        c.erase(cur_end, c.end());
    }

    template<class SeqContainer, typename... Rs>
    SeqContainer remove_seq_copy(const SeqContainer& c, Rs&&... rs)
    {
        SeqContainer result;
        for (const auto& e : c)
            if (!satisfies_one(e, rs...))
                result.push_back(e);
        return result;
    }

    template<typename Index, typename Tp, class F>
    std::pair<bool, Index> binary_search(Index left, Index right, Tp&& value, F&& f)
    {
        if (left >= right)
            return {true, left};
        return detail::binary_search_impl(left, --right, value, f);
    }

    class spinlock
    {
        std::atomic_flag flag_ = ATOMIC_FLAG_INIT;

    public:
        bool try_lock()
        {
            return !flag_.test_and_set(std::memory_order_acquire);
        }

        template<typename Period = std::micro>
        void lock(i64 interval = 0)
        {
            while (flag_.test_and_set(std::memory_order_acquire))
            {
                if (interval)
                    std::this_thread::sleep_for(std::chrono::duration<i64, Period>(interval));
                else
                    std::this_thread::yield();
            }
        }

        void unlock()
        {
            flag_.clear(std::memory_order_release);
        }
    };

    template<typename Tp, std::enable_if_t<std::is_floating_point_v<Tp>>>
    Tp from_string(const std::string& s)
    {
        return std::stold(s);
    }

    template<typename Tp, std::enable_if_t<std::is_integral_v<Tp> && std::is_signed_v<Tp>>>
    Tp from_string(const std::string& s)
    {
        return std::stoll(s);
    }

    template<typename Tp, std::enable_if_t<std::is_integral_v<Tp> && std::is_unsigned_v<Tp>>>
    Tp from_string(const std::string& s)
    {
        return std::stoull(s);
    }

    template<typename Tp>
    void from_string(const std::string& s, Tp& v)
    {
        v = from_string<Tp>(s);
    }

    template<class Tp>
    class data_view
    {
        const Tp* ptr_;
        u64 size_;

    public:
        template<typename Tp_>
        data_view(Tp_* ptr, u64 size = std::numeric_limits<u64>::max()) : ptr_(reinterpret_cast<const Tp*>(ptr)), size_(size) { }

        const Tp& operator[](u64 i) const noexcept
        {
            return ptr_[i];
        }

        const Tp& at(u64 i) const
        {
            if (i >= size_)
                throw std::out_of_range("data_view: Out of range");
            return ptr_[i];
        }

        u64 size() const noexcept
        {
            return size_;
        }
    };

    template<typename Tp_>
    data_view(Tp_*, u64) -> data_view<Tp_>;

    template<class Compare>
    class dereference_compare
    {
        const Compare comp_;

    public:
        template<class P1, class P2>
        bool operator()(const P1& a, const P2& b) const
        {
            return comp_(*a, *b);
        }
    };

    template<class Compare>
    class basic_compare
    {
    protected:
        const Compare comp_;

    public:
        basic_compare() = default;

        template<class Compare_>
        basic_compare(Compare_&& comp) : comp_(std::forward<Compare_>(comp)) { }
    };

    template<class Compare>
    class first_compare : public basic_compare<Compare>
    {
    public:
        using basic_compare<Compare>::basic_compare;

        template<typename P1, typename P2>
        bool operator()(const P1& a, const P2& b) const
        {
            return basic_compare<Compare>::comp_(a.first, b.first);
        }
    };

    template<class Compare>
    class second_compare : public basic_compare<Compare>
    {
    public:
        using basic_compare<Compare>::basic_compare;

        template<typename P1, typename P2>
        bool operator()(const P1& a, const P2& b) const
        {
            return basic_compare<Compare>::comp_(a.second, b.second);
        }
    };

    template<u64 N, class Compare>
    class nth_compare : public basic_compare<Compare>
    {
    public:
        using basic_compare<Compare>::basic_compare;

        template<typename E1, typename E2>
        bool operator()(const E1& a, const E2& b) const
        {
            return basic_compare<Compare>::comp_(std::get<N>(a), std::get<N>(b));
        }
    };
}
// namespace uf::utils
