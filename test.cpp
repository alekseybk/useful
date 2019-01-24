#include "useful/all.hpp"

#include <cassert>
#include <iostream>

#define assert_true(expr) assert(expr)
#define assert_false(expr) assert(!expr)

namespace uf::test
{
    namespace meta
    {
        using namespace uf::meta;

        void run_all_tests()
        {

        }
    }

    namespace utils
    {
        using namespace uf::utils;

        void satisfies_one_test()
        {
            {
                char c = 'a';
                assert_true(satisfies_one(c, 'a'));
                assert_true(satisfies_one(c, [](char c){ return c == 'a'; }));
                assert_true(satisfies_one(c, [](char c){ return false; }, 'a'));
                assert_false(satisfies_one(c, [](char c){ return false; }));
            }

            {
                int i = 5;
                assert_true(satisfies_one(i, 5));
                assert_true(satisfies_one(i, [](int i){ return i == 5; }));
                assert_true(satisfies_one(i, [](char c){ return false; }, 5));
                assert_false(satisfies_one(i, [](char c){ return false; }));
            }

            {
                string s = "12345";
                assert_true(satisfies_one(s, "12345"));
                assert_true(satisfies_one(s, [](const string& s){ return s == "12345"; }));
                assert_true(satisfies_one(s, [](const string&){ return false; }, [](const string& s){ return s == "12345"; }));
                assert_false(satisfies_one(s, [](const string&){ return false; }));
            }
        }

        void run_all_tests()
        {
            satisfies_one_test();
        }
    }

    void run_all_tests()
    {
        uf::test::meta::run_all_tests();
        uf::test::utils::run_all_tests();

        std::cout << "All tests passed" << std::endl;
    }
}


