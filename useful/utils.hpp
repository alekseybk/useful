#pragma once
#include "meta.hpp"

namespace uf
{
    namespace detail
    {
        template<typename Tp, u64... Ns>
        constexpr auto tuple_clone_value_helper(Tp&& value, sequence<Ns...>)
        {
            return std::make_tuple(mt::clone_ref_with_auto<Ns>(value)...);
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
    }
    // namespace detail

    inline namespace utils
    {
        template<typename Container, typename E, enif<std::is_lvalue_reference_v<Container>> = SF>
        constexpr auto&& forward_element(E&& e) noexcept
        {
            return std::forward<E>(e);
        }

        template<typename Container, typename E, enif<!std::is_reference_v<Container>> = SF>
        constexpr auto&& forward_element(E&& e) noexcept
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
        constexpr bool stf_any(const E& e, Ps&&... ps)
        {
            return (stf(e, ps) || ...);
        }

        template<class E, class... Ps>
        constexpr bool stf_all(const E& e, Ps&&... ps)
        {
            return (stf(e, ps) && ...);
        }

        template<class... Ps>
        constexpr auto stf_any_obj(Ps&&... ps)
        {
            return [ps...](const auto& e) mutable { return stf_any(e, ps...); }; // TODO: forward capture
        }

        template<class... Ps>
        constexpr auto stf_all_obj(Ps&&... ps)
        {
            return [ps...](const auto& e) mutable { return stf_all(e, ps...); }; // TODO: forward capture
        }

        template<typename P>
        auto stf_first_obj(P&& p)
        {
            return [p = std::forward<P>(p)](const auto& x) mutable { return stf(x.first, p); };
        }

        template<typename P>
        auto stf_second_obj(P&& p)
        {
            return [p = std::forward<P>(p)](const auto& x) mutable { return stf(x.second, p); };
        }

        template<u64 N, typename P>
        auto stf_nth_obj(P&& p)
        {
            return [p = std::forward<P>(p)](const auto& x) mutable { return stf(std::get<N>(x), p); };
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

        template<typename RandomAccessIterator, typename Compare>
        std::vector<u64> sort_indexes(RandomAccessIterator begin, RandomAccessIterator end, Compare&& cmp)
        {
            std::vector<u64> pos(std::distance(begin, end));
            std::iota(pos.begin(), pos.end(), 0);
            std::stable_sort(pos.begin(), pos.end(), [&](const auto& e1, const auto& e2)
            {
                return cmp(*( begin + e1), *(begin + e2));
            });
            return pos;
        }

        template<class Associative, class... Rs>
        void remove_associative(Associative& c, Rs&&... rs)
        {
            for (auto i = c.begin(); i != c.end();)
            {
                if (stf_any(*i, rs...))
                    i = c.erase(i);
                else
                    ++i;
            }
        }

        template<class Associative, class... Rs>
        Associative remove_associative_copy(const Associative& c, Rs&&... rs)
        {
            Associative result;
            for (auto i = c.begin(); i != c.end(); ++i)
                if (!stf_any(*i, rs...))
                    result.insert(*i);
            return result;
        }

        template<typename Index, typename Tp, class F>
        std::pair<bool, Index> binary_search_upper(Index begin, Index end, Tp&& value, F&& f)
        {
            const auto diff = end - begin;
            if (!diff)
                return {false, end};
            if (diff == 1)
                return {f(begin) == value, begin};
            if (diff == 2)
            {
                const auto x = f(begin + 1);
                if (x == value)
                    return {true, begin + 1};
                return {f(begin) == value, begin};
            }
            const Index middle = begin + (end - begin) / 2;
            if (value >= f(middle))
                return binary_search_upper(middle, end, value, f);
            return binary_search_upper(begin, middle, value, f);
        }

        template<typename Index, typename Tp, class F>
        std::pair<bool, Index> binary_search_lower(Index begin, Index end, Tp&& value, F&& f)
        {
            const auto diff = end - begin;
            if (!diff)
                return {false, end};
            if (diff == 1)
                return {f(begin) == value, begin};
            if (diff == 2)
            {
                const auto x = f(begin);
                if (x == value)
                    return {true, begin};
                return {f(begin + 1) == value, begin + 1};
            }
            const Index middle = begin + (end - begin) / 2;
            if (value <= f(middle))
                return binary_search_lower(begin, middle + 1, value, f);
            return binary_search_lower(middle + 1, end, value, f);
        }

        template<typename T, enif<std::is_pointer_v<std::decay_t<T>>> = SF>
        constexpr auto* get_base_ptr(T&& object)
        {
            return object;
        }

        template<typename T, enif<mt::is_legacy_iterator_v<std::decay_t<T>> && !std::is_pointer_v<std::decay_t<T>>> = SF>
        constexpr auto* get_base_ptr(T&& object)
        {
            return object.base();
        }

        template<typename T, enif<mt::is_reverse_iterator_v<std::decay_t<T>>> = SF>
        constexpr auto* get_base_ptr(T&& object)
        {
            return get_base_ptr(std::prev(object.base()));
        }

        template<typename T, enif<mt::is_smart_pointer_v<std::decay_t<T>>> = SF>
        constexpr auto* get_base_ptr(T&& object)
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

        template<class Container>
        class basic_reverse_wrapper;

        template<class Container>
        class basic_reverse_wrapper<Container&>
        {
        protected:
            Container& container_;

        public:
            basic_reverse_wrapper(Container& lref) : container_(lref) { }
        };

        template<class Container>
        class basic_reverse_wrapper<Container&&>
        {
        protected:
            Container container_;

        public:
            basic_reverse_wrapper(Container&& rref) : container_(std::move(rref)) { }
        };

        template<class Container>
        class reverse_wrapper : public basic_reverse_wrapper<Container>
        {
        public:
            template<typename C>
            reverse_wrapper(C&& container) : basic_reverse_wrapper<Container>(std::forward<C>(container)) { }

            auto begin()
            {
                return basic_reverse_wrapper<Container>::container_.rbegin();
            }

            auto end()
            {
                return basic_reverse_wrapper<Container>::container_.rend();
            }
        };

        template<typename C>
        reverse_wrapper(C&&) -> reverse_wrapper<C&&>;

        class spinlock
        {
            std::atomic_flag flag_ = ATOMIC_FLAG_INIT;

        public:
            bool try_lock()
            {
                return !flag_.test_and_set(std::memory_order_acquire);
            }

            template<typename Duration>
            void lock(Duration duration)
            {
                while (flag_.test_and_set(std::memory_order_acquire))
                    std::this_thread::sleep_for(duration);
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
