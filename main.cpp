/*
 * Copyright Aleksey Verkholat 2018 - ...
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include <bits/stdc++.h>

#include "useful.hpp"

using namespace std;

using namespace uf::basic_types;
using namespace uf::basic_types::literals;


int main()
{
    vector<int> a{1, 2, 3, 4};
    auto r = uf::split(a, [](const auto& c, auto i)
    {
        static_assert(std::is_same_v<decltype(c), const vector<int>&>);
        return *i == 3;
    });
    cout << r.size() << endl;
    return 0;
}
