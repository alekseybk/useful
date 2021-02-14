#pragma once
#include "meta.hpp"

namespace wh
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

    inline namespace ut
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
        constexpr bool stf_or(const E& e, Ps&&... ps)
        {
            return (stf(e, ps) || ...);
        }

        template<class E, class... Ps>
        constexpr bool stf_and(const E& e, Ps&&... ps)
        {
            return (stf(e, ps) && ...);
        }

        template<class... Ps>
        constexpr auto stf_or_obj(Ps&&... ps)
        {
            return [ps...](const auto& e) mutable { return stf_or(e, ps...); }; // TODO: forward captured values (C++20)
        }

        template<class... Ps>
        constexpr auto stf_and_obj(Ps&&... ps)
        {
            return [ps...](const auto& e) mutable { return stf_and(e, ps...); }; // TODO: forward captured values (C++20)
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

        template<typename T, enif<std::is_pointer_v<std::decay_t<T>>> = SF>
        constexpr auto* get_base_ptr(T&& object)
        {
            return object;
        }

        template<typename T, enif<mt::is_iterator_v<std::decay_t<T>> && !mt::is_reverse_iterator_v<std::decay_t<T>> && !std::is_pointer_v<std::decay_t<T>>> = SF>
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
        auto subtuple_include(T&& t)
        {
            return detail::subtuple_helper(std::forward<T>(t), sequence<Ns...>());
        }

        template<u64... Ns, typename T>
        auto subtuple_include_ref(T&& t)
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

        namespace detail
        {
            template<class Container>
            class basic_reverse_wrapper;

            template<class Container>
            class basic_reverse_wrapper<Container&>
            {
            protected:
                Container& m_container;

            public:
                basic_reverse_wrapper(Container& lref) : m_container(lref) { }
            };

            template<class Container>
            class basic_reverse_wrapper<Container&&>
            {
            protected:
                Container m_container;

            public:
                basic_reverse_wrapper(Container&& rref) : m_container(std::move(rref)) { }
            };

            template<class Container>
            class reverse_wrapper : public basic_reverse_wrapper<Container>
            {
            public:
                template<typename C>
                reverse_wrapper(C&& container) : basic_reverse_wrapper<Container>(std::forward<C>(container)) { }

                auto begin()
                {
                    return basic_reverse_wrapper<Container>::m_container.rbegin();
                }

                auto end()
                {
                    return basic_reverse_wrapper<Container>::m_container.rend();
                }
            };

            template<typename C>
            reverse_wrapper(C&&) -> reverse_wrapper<C&&>;
        }
        // namespace detail
        
        template<typename Container>
        auto reversed(Container&& container)
        {
            return detail::reverse_wrapper(std::forward<Container>(container));
        }

        class spinlock
        {
            std::atomic_flag m_flag = ATOMIC_FLAG_INIT;

        public:
            bool try_lock()
            {
                return !m_flag.test_and_set(std::memory_order_acquire);
            }

            template<typename Duration>
            void lock(Duration duration)
            {
                while (m_flag.test_and_set(std::memory_order_acquire))
                    std::this_thread::sleep_for(duration);
            }

            void lock()
            {
                while (m_flag.test_and_set(std::memory_order_acquire))
                    std::this_thread::yield();
            }

            void unlock()
            {
                m_flag.clear(std::memory_order_release);
            }
        };
    }
    // inline namespace ut
}
// namespace wh
