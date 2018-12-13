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
    cout << uf::utils::max(1, 2, 3, -1) << endl;
    return 0;
}
