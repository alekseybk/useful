/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "import.hpp"



namespace test::mt
{
    using namespace uf::mt;

    TEST(function_trait)
    {
        struct X { int f(double, char); };
        assert_eq(finfo<decltype(&X::f)>::arity, 2);
        assert_true((std::is_same_v<finfo<decltype(&X::f)>::result, int>));
        assert_true((std::is_same_v<finfo<decltype(&X::f)>::arg<0>, double>));
        assert_true((std::is_same_v<finfo<decltype(&X::f)>::arg<1>, char>));
    }
}
// namespace uf::test::meta
