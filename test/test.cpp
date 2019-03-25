/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include <iostream>
#include <map>
#include <functional>

std::multimap<std::string, std::function<void()>>& get_test_map()
{
    static std::multimap<std::string, std::function<void()>> result;
    return result;
}

void run_all_tests()
{
    std::cout << "Running " << get_test_map().size() << " tests...\n\n";
    for (auto& [name, func] : get_test_map())
    {
        std::cout << name;
        func();
        std::cout << ": OK" << std::endl;
    }
    std::cout << "All tests passed!" << std::endl << std::endl;
}

int main()
{
    run_all_tests();
}
