#include "testing.hpp"

#include "../useful/meta.hpp"

#include <memory>

using namespace uf::mt;
using namespace uf::base;

TEST(function_info)
{
    struct X { int f(double, char); };
    assert_eq(function_info<decltype(&X::f)>::arity, 2);
    assert_true((std::is_same_v<function_info<decltype(&X::f)>::result, int>));
    assert_true((std::is_same_v<function_info<decltype(&X::f)>::nth<0>, double>));
    assert_true((std::is_same_v<function_info<decltype(&X::f)>::nth<1>, char>));
}

TEST(is_iterator)
{
    assert_true(is_iterator_v<std::vector<int>::iterator>);
    assert_true((is_iterator_v<std::multimap<int, int>::iterator>));
    assert_false(is_iterator_v<std::shared_ptr<int>>);
    assert_false(is_iterator_v<int>);
}

TEST(is_iterable)
{
    assert_true(is_iterable_v<std::vector<int>>);
    assert_true(is_iterable_v<const std::vector<int>&>);
    assert_false(is_iterable_v<char>);
}

TEST(seq_reverse)
{
    assert_true((std::is_same_v<seq_reverse_t<sequence<1, 2, 3>>, seq_reverse_t<seq_reverse_t<sequence<3, 2, 1>>>>));
    assert_true((std::is_same_v<seq_reverse_t<sequence<3, 1, 2, 3, 2, 1, 6>>, sequence<6, 1, 2, 3, 2, 1, 3>>));
    assert_true((std::is_same_v<seq_reverse_t<sequence<3>>, sequence<3>>));
    assert_true((std::is_same_v<seq_reverse_t<sequence<>>, sequence<>>));
}

TEST(seq_remove)
{
    assert_true((std::is_same_v<seq_remove_t<sequence<1, 2, 3, 4, 1, 2, 5, 2, 3, 9, 1, 2, 6, 3>, 1, 2, 3, 4, 5, 6, 9>, sequence<>>));
    assert_true((std::is_same_v<seq_remove_t<sequence<1, 2, 3, 4, 1, 2, 5, 2, 3, 9, 1, 2, 6, 3>, 1, 2, 3, 4, 5, 6>, sequence<9>>));
    assert_false((std::is_same_v<seq_remove_t<sequence<1, 2, 3, 4, 1, 2, 5, 2, 3, 9, 1, 2, 6, 3>, 1, 2, 3, 4, 5, 6>, sequence<9, 2>>));
    assert_true((std::is_same_v<seq_remove_t<sequence<1>, 2, 3, 4, 5, 6>, sequence<1>>));
    assert_true((std::is_same_v<seq_remove_t<sequence<>, 2, 3, 4, 5, 6>, sequence<>>));
}

