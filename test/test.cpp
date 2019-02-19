/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#include "all.hpp"

std::vector<std::function<void()>>& get_test_vector()
{
    static std::vector<std::function<void()>> result;
    return result;
}

void run_all_tests()
{
    std::cout << "Running " << get_test_vector().size() << " tests..." << std::endl;
    for (auto& test : get_test_vector())
        test();
    std::cout << "All tests passed!" << std::endl << std::endl;
}

int main()
{
    run_all_tests();
}
