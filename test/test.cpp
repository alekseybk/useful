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
        func();
        std::cout << name << ": OK" << std::endl;
    }
    std::cout << "All tests passed!" << std::endl << std::endl;
}

int main()
{
    run_all_tests();
    return 0;
}
