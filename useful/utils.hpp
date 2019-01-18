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

        template<class Element, class P, class... Ps>
        bool satisfies_one(const Element& e, P&& p, Ps&&... ps)
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
        bool satisfies_all(const Element& e, Ps&&... ps)
        {
            return (satisfies_one(e, ps) && ...);
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

        template<class AssociativeContainer, class... Rs>
        void remove_associative_by_key(AssociativeContainer& c, Rs&&... rs)
        {
            detail::remove_associative_helper<true>(c, rs...);
        }

        template<class AssociativeContainer, class... Rs>
        void remove_associative_by_value(AssociativeContainer& c, Rs&&... rs)
        {
            detail::remove_associative_helper<false>(c, rs...);
        }

        template<class SeqContainer, typename... Ps>
        void remove_seq(SeqContainer& c, Ps&&... ps)
        {
            auto move_to = c.begin();
            for (auto& e : c)
                if (!satisfies_one(e, ps...))
                    *move_to++ = std::move(e);
            c.erase(move_to, c.end());
        }

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
    // namespace utils
}
// namespace uf
