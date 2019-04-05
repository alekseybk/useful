#include "testing.hpp"

#include "../useful/meta.hpp"
#include "../useful/traits.hpp"

#include <memory>

using namespace uf::mt;
using uf::traits;

TEST(function_trait)
{
    struct X { int f(double, char); };
    assert_eq(function_trait<decltype(&X::f)>::arity, 2);
    assert_true((std::is_same_v<function_trait<decltype(&X::f)>::result, int>));
    assert_true((std::is_same_v<function_trait<decltype(&X::f)>::nth<0>, double>));
    assert_true((std::is_same_v<function_trait<decltype(&X::f)>::nth<1>, char>));
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
    assert_true(traits<std::vector<int>>::is_iterable);
    assert_true(traits<const std::vector<int>&>::is_iterable);
}

