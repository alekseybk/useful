#include <bits/stdc++.h>

#include "useful.hpp"

using namespace std;

using namespace uf::basic_types;
using namespace uf::basic_types::literals;

void f(int)
{

}

int main()
{
    ifstream stream("/home/dbs/Downloads/akks11.txt");
    ofstream output_stream("/home/dbs/Downloads/akks11_output.txt");
    string s;
    while (((void)std::getline(stream, s), s.size()))
    {
        auto [l, p] = uf::split<2>(s, [](char c){return c == ':';});
        if (p.size() >= 10 && p.size() <= 15)
            output_stream << l << " : " << p << '\n';
    }
    output_stream << flush;
    return 0;
}
