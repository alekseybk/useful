#include <bits/stdc++.h>

#include "useful.hpp"

using namespace std;

using namespace uf::basic_types;
using namespace uf::basic_types::literals;

int main()
{

    std::map<int, int> a{{1, 2}, {4, 3}, {7, 2}};
    uf::remove_associative(a, [](int x){return x == 2;});
    for (auto [k, v] : a)
        cout << k << ' ' << v << endl;
    return 0;
}
