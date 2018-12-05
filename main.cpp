#include <bits/stdc++.h>

#include "useful.hpp"

using namespace std;

using namespace uf::basic_types;
using namespace uf::basic_types::literals;



auto& f(const int&a)
{
    return a;
}

auto& f(int& a)
{
    return a;
}

int main()
{
    int x = 1;
    static_assert (is_same_v<decltype(f(x)), int&>);

    string a = "123";
    const string b = "124";
    string c = "121";
    auto& cr = uf::min_ref(a, b, c);
    //cr[0] = '6';
    cout << c << endl;
    return 0;
}
