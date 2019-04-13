#pragma once
#include "meta.hpp"

namespace uf
{
    namespace detail
    {
        template<typename Tp, u64... Ns>
        constexpr auto tuple_clone_value_helper(Tp&& value, sequence<Ns...>)
        {
            return std::make_tuple(mt::clone_something<Ns>(value)...);
        }

        template<u64 N, typename F, typename... Ts>
        constexpr void tuple_for_each_helper(F&& f, Ts&&... ts)
        {
            if constexpr (N < std::tuple_size_v<mt::tpack_first_t<std::decay_t<Ts>...>>)
            {
                std::invoke(f, std::get<N>(std::forward<Ts>(ts))...);
                tuple_for_each_helper<N + 1>(f, std::forward<Ts>(ts)...);
            }
        }

        template<u64 N, typename F, typename... Ts>
        constexpr auto tuple_transform_helper2(F&& f, Ts&&... ts)
        {
            return std::invoke(f, std::get<N>(std::forward<Ts>(ts))...);
        }

        template<typename F, typename... Ts, u64... Ns>
        constexpr auto tuple_transform_helper(sequence<Ns...>, F&& f, Ts&&... ts)
        {
            return std::make_tuple(tuple_transform_helper2<Ns>(f, std::forward<Ts>(ts)...)...);
        }

        template<typename T, auto... Ns>
        constexpr auto subtuple_ref_helper(T&& t, sequence<Ns...>)
        {
            return std::tuple<decltype(std::get<Ns>(std::forward<T>(t)))...>(std::get<Ns>(std::forward<T>(t))...);
        }

        template<typename T, auto... Ns>
        constexpr auto subtuple_helper(T&& t, sequence<Ns...>)
        {
            return std::make_tuple(std::get<Ns>(std::forward<T>(t))...);
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

    inline namespace utils
    {
        template<typename Container, typename E, enif<std::is_lvalue_reference_v<Container>> = sdef>
        constexpr auto&& forward_element(E&& e)
        {
            return std::forward<E>(e);
        }

        template<typename Container, typename E, enif<!std::is_reference_v<Container>> = sdef>
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
            return detail::tuple_clone_value_helper(std::forward<Tp>(value), make_sequence<N>());
        }

        template<typename F, typename... Ts>
        constexpr void tuple_for_each(F&& f, Ts&&... ts)
        {
            constexpr u64 size = std::tuple_size_v<std::remove_reference_t<mt::tpack_first_t<Ts...>>>;
            static_assert (((std::tuple_size_v<std::remove_reference_t<Ts>> == size) && ...));
            detail::tuple_for_each_helper<0>(std::forward<F>(f), std::forward<Ts>(ts)...);
        }

        template<typename F, typename... Ts>
        constexpr auto tuple_transform(F&& f, Ts&&... ts)
        {
            constexpr u64 size = std::tuple_size_v<std::remove_reference_t<mt::tpack_first_t<Ts...>>>;
            static_assert (((std::tuple_size_v<std::remove_reference_t<Ts>> == size) && ...));
            return detail::tuple_transform_helper(make_sequence<size>(), std::forward<F>(f), std::forward<Ts>(ts)...);
        }

        template<class R, class SeqContainer>
        R add_positions(SeqContainer&& x)
        {
            R result;
            result.reserve(x.size());
            u64 j = 0;
            for (auto i = x.begin(); i != x.end(); ++i, ++j)
                result.emplace_back(j, forward_element<SeqContainer>(*i));
            return result;
        }

        template<class SeqContainer>
        auto add_positions(SeqContainer&& x)
        {
            return add_positions<mt::instance_info<std::decay_t<SeqContainer>>::template type<std::pair<u64, typename std::decay_t<SeqContainer>::value_type>>>>(std::forward<SeqContainer>(x));
        }

        template<class SeqContainer, typename Compare>
        auto add_positions_and_sort(SeqContainer&& c, Compare&& compare)
        {
            auto result = add_positions(std::forward<SeqContainer>(c));
            std::sort(result.begin(), result.end(), [&compare](const auto& e1, const auto& e2)
            {
                return compare(e1.second, e2.second);
            });
            return result;
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
            for (auto i = c.begin(); i != c.end();)
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
            for (auto i = c.begin(); i != c.end(); ++i)
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

        template<typename T>
        auto* get_underlying_ptr(T* object)
        {
            return object;
        }

        template<typename T, enif<mt::is_iterator_v<std::decay_t<T>>> = sdef, disif<mt::is_instantiated_from_v<std::reverse_iterator, std::remove_reference_t<T>>> = sdef>
        auto* get_underlying_ptr(T&& object)
        {
            return object.base();
        }

        template<typename T, enif<mt::is_instantiated_from_v<std::reverse_iterator, std::remove_reference_t<T>>> = sdef>
        auto* get_underlying_ptr(T&& object)
        {
            auto b = object.base();
            return get_underlying_ptr(--b);
        }

        template<typename T, enif<uf::mt::is_smart_pointer_v<std::decay_t<T>>> = sdef>
        auto* get_underlying_ptr(T&& object)
        {
            return object.get();
        }

        template<u64 B = 0, u64 E = std::numeric_limits<u64>::max(), typename T>
        auto subtuple_ref(T&& t)
        {
            constexpr u64 size = std::tuple_size_v<std::remove_reference_t<T>>;
            if constexpr (E > size)
                return detail::subtuple_ref_helper(std::forward<T>(t), mt::seq_increasing_t<B, size>());
            else
                return detail::subtuple_ref_helper(std::forward<T>(t), mt::seq_increasing_t<B, E>());
        }

        template<u64 B = 0, u64 E = std::numeric_limits<u64>::max(), typename T>
        auto subtuple(T&& t)
        {
            constexpr u64 size = std::tuple_size_v<std::remove_reference_t<T>>;
            if constexpr (E > size)
                return detail::subtuple_helper(std::forward<T>(t), mt::seq_increasing_t<B, size>());
            else
                return detail::subtuple_helper(std::forward<T>(t), mt::seq_increasing_t<B, E>());
        }

        template<u64... Ns, typename T>
        auto subtuple_only(T&& t)
        {
            return detail::subtuple_helper(std::forward<T>(t), sequence<Ns...>());
        }

        template<u64... Ns, typename T>
        auto subtuple_only_ref(T&& t)
        {
            return detail::subtuple_ref_helper(std::forward<T>(t), sequence<Ns...>());
        }

        template<u64... Ns, typename T>
        auto subtuple_exclude(T&& t)
        {
            return detail::subtuple_helper(std::forward<T>(t), mt::seq_remove_t<make_sequence<std::tuple_size_v<std::decay_t<T>>>, Ns...>());
        }

        template<u64... Ns, typename T>
        auto subtuple_exclude_ref(T&& t)
        {
            return detail::subtuple_ref_helper(std::forward<T>(t), mt::seq_remove_t<make_sequence<std::tuple_size_v<std::decay_t<T>>>, Ns...>());
        }

        template<typename Tp>
        auto get_unique(Tp&& obj)
        {
            return std::make_unique<std::remove_reference_t<Tp>>(std::forward<Tp>(obj));
        }

        template<typename Tp>
        auto get_shared(Tp&& obj)
        {
            return std::make_shared<std::remove_reference_t<Tp>>(std::forward<Tp>(obj));
        }

        class spinlock
        {
            std::atomic_flag flag_ = ATOMIC_FLAG_INIT;

        public:
            bool try_lock()
            {
                return !flag_.test_and_set(std::memory_order_acquire);
            }

            template<typename Period>
            void lock(i64 count)
            {
                while (flag_.test_and_set(std::memory_order_acquire))
                    std::this_thread::sleep_for(std::chrono::duration<i64, Period>(count));
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
    // inline namespace utils
}
// namespace uf
