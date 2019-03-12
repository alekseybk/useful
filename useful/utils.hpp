/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "meta.hpp"

namespace uf
{
    namespace detail
    {
        template<typename Tp, u64... Ns>
        constexpr auto tuple_clone_value_helper(Tp&& value, std::index_sequence<Ns...>)
        {
            return std::make_tuple(mt::clone_ref<Ns>(value)...);
        }

        template<typename T, typename F, u64... Ns>
        constexpr void tuple_for_each_helper(T&& t, F&& f, std::index_sequence<Ns...>)
        {
            (std::invoke(f, std::get<Ns>(std::forward<T>(t))), ...);
        }

        template<u64 N, u64... Ns, typename Tp, typename F>
        constexpr auto tuple_transform_only_helper2(Tp&& value, F&& f)
        {
            if constexpr (((N == Ns) || ...))
                return std::invoke(f, std::forward<Tp>(value));
            else
                return u8(0);
        }

        template<u64 N, u64... Ns, typename Tp, typename F>
        constexpr auto tuple_transform_exclude_helper2(Tp&& value, F&& f)
        {
            if constexpr (((N != Ns) && ...))
                return std::invoke(f, std::forward<Tp>(value));
            else
                return u8(0);
        }

        template<typename T, typename F, u64... SArgs>
        constexpr auto tuple_transform_helper(T&& t, F&& f, std::index_sequence<SArgs...>)
        {
            return std::make_tuple(std::invoke(f, std::get<SArgs>(std::forward<T>(t)))...);
        }

        template<typename T, typename F, u64... SArgs, u64... Ns>
        constexpr auto tuple_transform_only_helper(T&& t, F&& f, std::index_sequence<SArgs...>, std::index_sequence<Ns...>)
        {
            return std::make_tuple(tuple_transform_only_helper2<SArgs, Ns...>(std::get<SArgs>(t), f)...);
        }

        template<typename T, typename F, u64... SArgs, u64... Ns>
        constexpr auto tuple_transform_exclude_helper(T&& t, F&& f, std::index_sequence<SArgs...>, std::index_sequence<Ns...>)
        {
            return std::make_tuple(tuple_transform_exclude_helper2<SArgs, Ns...>(std::get<SArgs>(t), f)...);
        }

        template<typename Index, typename Tp, class F>
        Index binary_search_impl(Index left, Index right, Index end, Tp&& value, F&& f)
        {
            const auto diff = right - left;
            if (diff <= 1)
            {
                if (f(right) == value)
                    return right;
                if (f(left) == value)
                    return left;
                return end;
            }
            const Index middle = left + diff / 2;
            if (f(middle) < value)
                return binary_search_impl(middle, right, end, std::forward<Tp>(value), std::forward<F>(f));
            return binary_search_impl(left, middle, end, std::forward<Tp>(value), std::forward<F>(f));
        }
    }
    // namespace detail

    template<typename Owner, typename E, enif<std::is_lvalue_reference_v<Owner>> = sdef>
    constexpr auto&& forward_element(E&& e)
    {
        return std::forward<E>(e);
    }

    template<typename Owner, typename E, enif<!std::is_reference_v<Owner>> = sdef>
    constexpr auto&& forward_element(E&& e)
    {
        return std::move(e);
    }

    template<typename E, typename P>
    constexpr bool stf(const E& e, P&& p)
    {
        if constexpr (std::is_invocable_v<std::remove_reference_t<P>, const E&>)
            return std::invoke(p, e);
        else
            return e == p;
    }

    template<class E, class... Ps>
    constexpr bool stf_one(const E& e, Ps&&... ps)
    {
        return (stf(e, ps) || ...);
    }

    template<class E, class... Ps>
    constexpr bool stf_all(const E& e, Ps&&... ps)
    {
        return (stf(e, ps) && ...);
    }

    template<class... Ps>
    constexpr auto stf_one_obj(Ps&&... ps)
    {
        return std::bind([](const auto& e, Ps&... ps){ return stf_one(e, ps...); }, std::placeholders::_1, std::forward<Ps>(ps)...);
    }

    template<class... Ps>
    constexpr auto stf_all_obj(Ps&&... ps)
    {
        return std::bind([](const auto& e, Ps&... ps){ return stf_all(e, ps...); }, std::placeholders::_1, std::forward<Ps>(ps)...);
    }

    template<u64 N, typename Tp>
    constexpr auto tuple_clone_value(Tp&& value)
    {
        return detail::tuple_clone_value_helper(std::forward<Tp>(value), std::make_index_sequence<N>());
    }

    template<typename T, typename F>
    constexpr void tuple_for_each(T&& t, F&& f)
    {
        detail::tuple_for_each_helper(std::forward<T>(t), std::forward<F>(f), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>());
    }

    template<u64... Ns, typename T, typename F>
    constexpr void tuple_for_each_only(T&& t, F&& f)
    {
        detail::tuple_for_each_helper(std::forward<T>(t), std::forward<F>(f), std::index_sequence<Ns...>());
    }

    template<u64... Ns, typename T, typename F>
    constexpr void tuple_for_each_exclude(T&& t, F&& f)
    {
        detail::tuple_for_each_helper(std::forward<T>(t), std::forward<F>(f), mt::seq_remove_t<std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>(), Ns...>());
    }

    template<typename T, typename F>
    constexpr auto tuple_transform(T&& t, F&& f)
    {
        return tuple_transform_helper(std::forward<T>(t), std::forward<F>(f), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>());
    }

    template<u64... Ns, typename T, typename F>
    constexpr auto tuple_transform_only(T&& t, F&& f)
    {
        if constexpr (sizeof...(Ns))
            return tuple_transform_helper(std::forward<T>(t), std::forward<F>(f), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>(), std::index_sequence<Ns...>());
        else
            return tuple_clone_value<std::tuple_size_v<std::remove_reference_t<T>>>(u8(0));
    }

    template<u64... Ns, typename T, typename F>
    constexpr auto tuple_transform_exclude(T&& t, F&& f)
    {
        if constexpr (sizeof...(Ns))
            return tuple_transform_exclude_helper(std::forward<T>(t), std::forward<F>(f), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>(), std::index_sequence<Ns...>());
        else
            return tuple_transform(std::forward<T>(t), std::forward<F>(f));
    }

    template<class R, class SeqContainer>
    R add_positions(SeqContainer&& x)
    {
        R result;
        u64 j = 0;
        for (auto i = x.begin(); i != x.end(); ++i, ++j)
            result.push_back(std::make_pair(j, forward_element<SeqContainer>(*i)));
        return result;
    }

    template<class SeqContainer>
    auto add_positions(SeqContainer&& x)
    {
        using decayed = std::decay_t<SeqContainer>;
        using value_type = typename decayed::value_type;
        using result_type = typename mt::clean<decayed>::template type<std::pair<u64, value_type>>;
        return add_positions<result_type>(x);
    }

    template<typename Tp, typename C, typename F>
    void for_each_trg(C&& c, F&& f)
    {
        using value_type = typename std::decay_t<C>::value_type;

        if constexpr (std::is_same_v<Tp, value_type>)
            std::for_each(c.begin(), c.end(), f);
        else
            for (auto& value : c)
                for_each_trg<Tp>(forward_element<C>(value), f);
    }

    template<typename P>
    auto stf_first_obj(P&& p)
    {
        return [p = std::forward<P>(p)](const auto& x) mutable -> bool { return stf(x.first, p); };
    }

    template<typename P>
    auto stf_second_obj(P&& p)
    {
        return [p = std::forward<P>(p)](const auto& x) mutable -> bool { return stf(x.second, p); };
    }

    template<u64 N, typename P>
    auto stf_nth_obj(P&& p)
    {
        return [p = std::forward<P>(p)](const auto& x) mutable -> bool { return stf(std::get<N>(x), p); };
    }

    template<class AssocContainer, class... Rs>
    void remove_associative(AssocContainer& c, Rs&&... rs)
    {
        for (auto i = c.cbegin(); i != c.cend();)
        {
            if (stf_one(*i, rs...))
                i = c.erase(i);
            else
                ++i;
        }
    }

    template<class AssocContainer, class... Rs>
    AssocContainer remove_associative_copy(const AssocContainer& c, Rs&&... rs)
    {
        AssocContainer result;
        for (auto i = c.cbegin(); i != c.cend(); ++i)
            if (!stf_one(*i, rs...))
                result.insert(*i);
        return result;
    }

    template<typename Index, typename Tp, class F>
    Index binary_search(Index left, Index right, Tp&& value, F&& f)
    {
        if (left >= right)
            return right;
        return detail::binary_search_impl(left, right - 1, right, value, std::forward<F>(f));
    }

    template<typename Tp>
    class ptr_and_size
    {
    public:
        using element_type = Tp;

    private:
        element_type* ptr_;
        u64 size_;

    public:
        ptr_and_size(element_type* ptr, u64 size) : ptr_(ptr), size_(size) { }

        element_type& operator[](u64 i) const noexcept
        {
            return ptr_[i];
        }

        element_type& at(u64 i) const
        {
            if (i >= size_)
                throw std::out_of_range("ptr_and_size: Out of range");
            return ptr_[i];
        }

        element_type* ptr() const noexcept
        {
            return ptr_;
        }

        u64 size() const noexcept
        {
            return size_;
        }
    };

    template<typename Tp>
    ptr_and_size(Tp*, u64) -> ptr_and_size<Tp>;

    class spinlock
    {
        std::atomic_flag flag_ = ATOMIC_FLAG_INIT;

    public:
        bool try_lock()
        {
            return !flag_.test_and_set(std::memory_order_acquire);
        }

        template<typename Period, i64 Count>
        void lock()
        {
            while (flag_.test_and_set(std::memory_order_acquire))
                std::this_thread::sleep_for(std::chrono::duration<i64, Period>(Count));
        }

        void lock()
        {
            while (flag_.test_and_set(std::memory_order_acquire))
                std::this_thread::yield();
        }

        void unlock()
        {
            flag_.clear(std::memory_order_release);
        }
    };
}
// namespace uf
