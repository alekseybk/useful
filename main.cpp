/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include <iostream>
#include <fstream>
#include <set>
#include <thread>
#include <mutex>

#include "useful/useful.hpp"
#include "useful/out_overloads.hpp"
#include "useful/in_overloads.hpp"

#define check_types_equal(t1, t2) static_assert(std::is_same_v<t1, t2>)
#define check_expr_type(expression, type) static_assert(std::is_same_v<decltype(expression), type>)

using namespace std;

using namespace uf::out_overloads;
using namespace uf::in_overloads;



int main()
{
    uf::spinlock sl;
    int i = 0;

    thread t2([&sl, &i]()
    {
        this_thread::sleep_for(chrono::milliseconds(1));
        while (true)
        {
            sl.lock(500000);
            if (i == 1000)
            {
                sl.unlock();
                return;
            }
            cout << ++i << " t2" << endl;
            sl.unlock();
        }
    });

    thread t1([&sl, &i]()
    {
        this_thread::sleep_for(chrono::milliseconds(1));
        while (true)
        {
            sl.lock<std::deci>(1);
            if (i == 1000)
            {
                sl.unlock();
                return;
            }
            cout << ++i << " t1" << endl;
            sl.unlock();
        }
    });

    t1.join();
    t2.join();

    return 0;
}
