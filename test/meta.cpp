#include "testing.hpp"

#include "../useful/meta.hpp"

#include <memory>

using namespace uf::mt;
using namespace uf::base;

TEST(function_info)
{
    struct X { int f(double, char); };
    static_assert (function_info<decltype(&X::f)>::arity == 2);
    static_assert (std::is_same_v<function_info<decltype(&X::f)>::result, int>);
    static_assert (std::is_same_v<function_info<decltype(&X::f)>::nth<0>, double>);
    static_assert (std::is_same_v<function_info<decltype(&X::f)>::nth<1>, char>);
}

TEST(is_iterator)
{
    static_assert (is_iterator_v<std::vector<int>::iterator>);
    static_assert (is_iterator_v<std::multimap<int, int>::iterator>);
    static_assert (!is_iterator_v<std::shared_ptr<int>>);
    static_assert (!is_iterator_v<int>);
}

TEST(is_iterable)
{
    static_assert (is_iterable_v<std::vector<int>>);
    static_assert (is_iterable_v<const std::vector<int>&>);
    static_assert (!is_iterable_v<char>);
}

TEST(seq_reverse)
{
    static_assert (std::is_same_v<seq_reverse_t<sequence<1, 2, 3>>, seq_reverse_t<seq_reverse_t<sequence<3, 2, 1>>>>);
    static_assert (std::is_same_v<seq_reverse_t<sequence<3, 1, 2, 3, 2, 1, 6>>, sequence<6, 1, 2, 3, 2, 1, 3>>);
    static_assert (std::is_same_v<seq_reverse_t<sequence<3>>, sequence<3>>);
    static_assert (std::is_same_v<seq_reverse_t<sequence<>>, sequence<>>);
}

TEST(seq_remove)
{
    static_assert (std::is_same_v<seq_remove_t<sequence<1, 2, 3, 4, 1, 2, 5, 2, 3, 9, 1, 2, 6, 3>, 1, 2, 3, 4, 5, 6, 9>, sequence<>>);
    static_assert (std::is_same_v<seq_remove_t<sequence<1, 2, 3, 4, 1, 2, 5, 2, 3, 9, 1, 2, 6, 3>, 1, 2, 3, 4, 5, 6>, sequence<9>>);
    static_assert (!std::is_same_v<seq_remove_t<sequence<1, 2, 3, 4, 1, 2, 5, 2, 3, 9, 1, 2, 6, 3>, 1, 2, 3, 4, 5, 6>, sequence<9, 2>>);
    static_assert (std::is_same_v<seq_remove_t<sequence<1>, 2, 3, 4, 5, 6>, sequence<1>>);
    static_assert (std::is_same_v<seq_remove_t<sequence<>, 2, 3, 4, 5, 6>, sequence<>>);
}

TEST(seq_one_value)
{
    static_assert (std::is_same_v<seq_one_value_t<3, 4>, sequence<3, 3, 3, 3>>);
    static_assert (std::is_same_v<seq_one_value_t<3, 0>, sequence<>>);
}

TEST(tuple_one_type)
{
    static_assert (std::is_same_v<tuple_one_type_t<int, 4>, std::tuple<int, int, int, int>>);
    static_assert (std::is_same_v<tuple_one_type_t<int, 0>, std::tuple<>>);
}

TEST(seq_select)
{
    static_assert (std::is_same_v<seq_select_t<sequence<1, 2, 3>, 2, 0>, sequence<3, 1>>);
    static_assert (std::is_same_v<seq_select_t<sequence<1, 2, 3>>, sequence<>>);
}

TEST(tpack_last)
{
    static_assert (std::is_same_v<tpack_last_t<int, char>, char>);
    static_assert (std::is_same_v<tpack_last_t<int, char&>, char&>);
    static_assert (std::is_same_v<tpack_last_t<int, char&&>, char&&>);
    static_assert (std::is_same_v<tpack_last_t<char>, char>);
    static_assert (std::is_same_v<tpack_last_t<char&>, char&>);
    static_assert (std::is_same_v<tpack_last_t<char&&>, char&&>);
}


