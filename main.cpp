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
    Nullable nl = NoDefCtr(10);
    cout << nl().get() << endl;
    +nl;
    cout << ~nl << endl;
    return 0;
}
