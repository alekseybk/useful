#include <bits/stdc++.h>

#include "useful.hpp"

using namespace std;

using namespace uf::basic_types;
using namespace uf::basic_types::literals;


int main()
{
    cout << uf::meta::is_string_type_v<const char(&)[6]> << endl;
    string s("111 222, 333");
    for (const auto& ss : uf::split(s, ' ', ','))
        cout << ss << endl;
    return 0;
}
