/*
 * Copyright Aleksey Verkholat 2018 - ...
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include <bits/stdc++.h>

#include "useful.hpp"

using namespace std;

using namespace uf::short_integer_types;
using namespace uf::short_integer_types::literals;

using namespace uf::print_overloads;

int main()
{

    vector<int> v{1, 2, 3};
    cout << v << endl;

    pair<int, double> p{1, 2.2};
    cout << p << endl;
    tuple<int, double> t{5, 5.5};
    cout << t << endl;
    vector<pair<char, char>> v2{{'x', 'x'}};
    cout << v2 << endl;

    //cout << uf::print_overloads::detail::is_container_v<int> << endl;
    return 0;
}
