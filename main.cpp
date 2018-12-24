/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include "useful/useful.hpp"

#include <iostream>

#define check_types_equal(t1, t2) static_assert(std::is_same_v<t1, t2>)
#define check_expr_type(expression, type) static_assert(std::is_same_v<decltype(expression), type>)

using namespace std;

int main()
{
    string s = "123,123,123,444 ";
    string s2 = uf::strip(s, ',', ' ', '2');
    cout << s2 << endl;
    cout << s << endl;
    return 0;
}
