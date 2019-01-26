/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "meta.hpp"

namespace uf
{
    namespace utils
    {
        template<class E, class... Ps>
        bool satisfies_one(const E& e, Ps&&... ps);

        namespace detail
        {
            template<bool Result, typename Tuple, typename F, u64... Ns>
            constexpr auto tuple_for_each_helper(Tuple&& t, F&& f, index_sequence<Ns...>)
            {
                if constexpr (Result)
                    return tuple{f(std::get<Ns>(std::forward<Tuple>(t)))...};
                else
                    (f(std::get<Ns>(std::forward<Tuple>(t))), ...);
            }

            template<bool ByKey, typename E, class... Rs>
            bool remove_assoc_element_check(const E& e, Rs&&... rs)
            {
                if constexpr (meta::is_pair_v<E>)
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
        }
        // namespace detail

        template<typename Owner, typename E, enable_if_t<is_lvalue_reference_v<Owner>, int> = 0>
        constexpr auto&& forward_element(E&& e)
        {
            return std::forward<E>(e);
        }

        template<typename Owner, typename E, disable_if_t<is_reference_v<Owner>, int> = 0>
        constexpr auto&& forward_element(E&& e)
        {
            return std::move(e);
        }

        template<class SeqContainer>
        auto position_pairs(SeqContainer&& container)
        {
            using value_type = typename remove_reference_t<SeqContainer>::value_type;
            using result_type = typename meta::clean<remove_reference_t<SeqContainer>>::template type<pair<u64, value_type>>;

            u64 j = 0;
            result_type result;
            for (auto i = container.begin(); i != container.end(); ++i, ++j)
                result.push_back({j, utils::forward_element<SeqContainer>(*i)});
            return result;
        }

        template<class SeqContainer, class Compare>
        auto sort_save_position(SeqContainer&& container, const Compare& comp)
        {
            auto pairs = position_pairs(std::forward<SeqContainer>(container));
            sort(pairs.begin(), pairs.end(), [&comp](const auto& a, const auto& b)
            {
                return comp(a.second, b.second);
            });
            return pairs;
        }

        template<class E, class... Ps>
        bool satisfies_one(const E& e, Ps&&... ps)
        {
            const auto check_function = [&e](auto& predicate)
            {
                if constexpr (is_invocable_v<remove_reference_t<decltype(predicate)>, const E&>)
                    return predicate(e);
                else
                    return e == predicate;
            };
            return (check_function(ps) || ...);
        }

        template<class E, class... Ps>
        bool satisfies_all(const E& e, Ps&&... ps)
        {
            const auto check_function = [&e](auto& predicate)
            {
                if constexpr (is_invocable_v<remove_reference_t<decltype(predicate)>, const E&>)
                    return predicate(e);
                else
                    return e == predicate;
            };
            return (check_function(ps) && ...);
        }

        template<bool Result = false, typename Tuple, typename F>
        constexpr auto tuple_for_each(Tuple&& t, F&& f)
        {
            using tuple_type = remove_reference_t<Tuple>;

            if constexpr (Result)
                return detail::tuple_for_each_helper<Result>(std::forward<Tuple>(t), f, make_index_sequence<tuple_size_v<tuple_type>>());
            else
                detail::tuple_for_each_helper<Result>(std::forward<Tuple>(t), f, make_index_sequence<tuple_size_v<tuple_type>>());
        }

        template<typename Target, typename Container, typename F>
        void targeted_for_each(Container&& c, F&& f)
        {
            using value_type = decay_t<typename remove_reference_t<Container>::value_type>;

            if constexpr (is_same_v<decay_t<Target>, value_type>)
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

        class spinlock
        {
            atomic_flag flag_ = ATOMIC_FLAG_INIT;

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
                        this_thread::sleep_for(chrono::duration<i64, Period>(interval));
                    else
                        this_thread::yield();
                }
            }

            void unlock()
            {
                flag_.clear(std::memory_order_release);
            }
        };

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
        class first_compare
        {
            const Compare comp_;

        public:
            template<typename P1, typename P2>
            bool operator()(const P1& a, const P2& b) const
            {
                return comp_(a.first, b.first);
            }
        };

        template<class Compare>
        class second_compare
        {
            const Compare comp_;

        public:
            template<typename P1, typename P2>
            bool operator()(const P1& a, const P2& b) const
            {
                return comp_(a.second, b.second);
            }
        };
    }
    // namespace utils
}
// namespace uf
