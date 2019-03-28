/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include "testing.hpp"

#include "../useful/meta.hpp"

#include <memory>

using namespace uf::mt;

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

