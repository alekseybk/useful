/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include <iostream>
#include <fstream>
#include <set>

#include "useful/useful.hpp"
#include "useful/out_overloads.hpp"
#include "useful/in_overloads.hpp"

#define check_types_equal(t1, t2) static_assert(std::is_same_v<t1, t2>)
#define check_expr_type(expression, type) static_assert(std::is_same_v<decltype(expression), type>)

using namespace std;

using namespace uf::out_overloads;
using namespace uf::in_overloads;



int main()
{
    //uf::utils::for_each_target<string>(uf::read_csv(ifstream("test.csv")), [](string& s){ s = uf::strip_sides(s, ' '); cout << s << endl; });

    std::set<int> m{1, 2, 3, 4, 5};
    uf::remove_associative_by_value(m, [](auto a){return a % 2 == 0;}, 1);
    cout << m << endl;

    //out << uf::split("123 124,125"s, ' ') << endl;

    return 0;
}
