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
    vector<tuple<double, char>> x{{1.0, 'b'}, {5.5, 'x'}};
    map<int, double> m;
    m.insert({1, 4.4});
    cout << m << endl << x << endl;
    return 0;
}
