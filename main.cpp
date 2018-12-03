#include <bits/stdc++.h>

#include "useful.hpp"

using namespace std;

using namespace uf::basic_types;
using namespace uf::basic_types::literals;

int main()
{
    int i = 0;
    uf::spinlock sl;

    thread t2([&sl, &i]()
    {
        while (true)
        {
            std::lock_guard lg(sl);
            if (i >= 250)
            {
                cout << "2 END\n";
                return;
            }
            cout << "2 -> ";
            cout << i++ << endl;
        }
    });

    while (true)
    {
        std::lock_guard lg(sl);
        if (i >= 100)
        {
            cout << "1 END\n";
            break;
        }
        cout << "1 -> ";
        cout << i++ << endl;
    }
    t2.join();

    return 0;
}
