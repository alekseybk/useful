/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include <iostream>
#include <fstream>

#include "useful/basic_types.hpp"
#include "useful/meta.hpp"
#include "useful/useful.hpp"
#include "useful/utils.hpp"
#include "useful/output.hpp"
#include "useful/input.hpp"
#include "useful/time_meter.hpp"

#define check_types_equal(t1, t2) static_assert(std::is_same_v<t1, t2>)
#define check_expr_type(expression, type) static_assert(std::is_same_v<decltype(expression), type>)

using namespace std;

using namespace uf::output;
using namespace uf::input;
using namespace uf::basic_types;
using namespace uf::basic_types::literals;

int main()
{
    cout << 2.1F << endl;
    return 0;
}
