#include <iostream>
#include <array>

#include "useful.hpp"

using namespace std;

using uf::cls::Nullable;

class NoDefCtr
{
    int i;
public:
    int get() const {return i;}
    NoDefCtr(int i_) : i(i_) {}
};

int main()
{
    const NoDefCtr a(10);
    Nullable nl = a;
    -nl;
    nl();
    return 0;
}
