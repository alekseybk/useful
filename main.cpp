#include <bits/stdc++.h>

#include "useful.hpp"

using namespace std;

using namespace uf::basic_types;
using namespace uf::basic_types::literals;

int main()
{
    string s = "14.123, 124; 125. 11.12.13";
    auto sv = uf::split(s, [](char c){return c == ' ';}, ',', ';', [](char c){return c == '.';});
    for (auto ss : sv)
        cout << ss << endl;
    return 0;
}
