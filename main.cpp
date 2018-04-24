#include <iostream>

#include "useful.hpp"

using namespace std;
using namespace useful;

int main()
{
    cout << meta::is_string_type_v<const string_view> << endl;
    return 0;
}
