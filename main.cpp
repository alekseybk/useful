/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include <iostream>
#include <fstream>
#include <set>
#include <thread>
#include <mutex>

#include "useful/useful.hpp"
#include "useful/out_overloads.hpp"
#include "useful/in_overloads.hpp"

#define check_types_equal(t1, t2) static_assert(std::is_same_v<t1, t2>)
#define check_expr_type(expression, type) static_assert(std::is_same_v<decltype(expression), type>)

using namespace std;

using namespace uf::out_overloads;
using namespace uf::in_overloads;

using namespace uf::short_int;
using namespace uf::short_int::literals;

int main()
{
    void* x = 0x1_ptr;
    i32* a = 0x2_i32p;
    return 0;
}
