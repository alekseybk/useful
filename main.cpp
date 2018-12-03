#include <bits/stdc++.h>

#include "useful.hpp"

using namespace std;

using namespace uf::basic_types;
using namespace uf::basic_types::literals;

int main()
{
    cout << uf::meta::is_same_tmpl_v<map, unordered_map<int, double>> << endl;
    return 0;
}
