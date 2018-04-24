#include <iostream>

#include "useful.hpp"

using namespace std;
using namespace useful;

int main()
{
    try
    {
        fn::err("123");
    }
    catch (std::runtime_error& e)
    {
        cout << e.what() << endl;
    }
    return 0;
}
