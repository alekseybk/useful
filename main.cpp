/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include <iostream>
#include <fstream>

#include "useful/all.hpp"

#define check_equality(t1, t2) static_assert(std::is_same_v<t1, t2>)
#define check_expr(expression, type) static_assert(std::is_same_v<decltype(expression), type>)

using namespace std;

using namespace uf;
using namespace uf::output;
using namespace uf::input;

struct A
{
    int value;
    A(int v = 0) : value(v) { cout << __PRETTY_FUNCTION__ << endl; }
    A(const A&) { cout << __PRETTY_FUNCTION__ << endl; }
    A(A&&) { cout << __PRETTY_FUNCTION__ << endl; }
    A& operator=(A&&) { cout << __PRETTY_FUNCTION__ << endl; return *this; }
    A& operator=(const A&) { cout << __PRETTY_FUNCTION__ << endl; return *this; }
    bool operator>(const A& a) { return value > a.value; }
};

int main()
{
    cout << uf::ends_with("123"s, "13"s) << endl;
    return 0;
}
