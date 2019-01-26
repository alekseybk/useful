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
                assert_true(satisfies_one(c, [](char){ return false; }, 'a'));
                assert_false(satisfies_one(c, [](char){ return false; }));
            }

            {
                int i = 5;
                assert_true(satisfies_one(i, 5));
                assert_true(satisfies_one(i, [](int i){ return i == 5; }));
                assert_true(satisfies_one(i, [](int){ return false; }, 5));
                assert_false(satisfies_one(i, [](int){ return false; }));
            }

            {
                string s = "12345";
                assert_true(satisfies_one(s, "12345"));
                assert_true(satisfies_one(s, [](const string& s){ return s == "12345"; }));
                assert_true(satisfies_one(s, [](const string&){ return false; }, [](const string& s){ return s == "12345"; }));
                assert_false(satisfies_one(s, [](const string&){ return false; }));
            }
        }

        void remove_assoc_key_test()
        {
            {
                map<string, int> m{{"123", 123}, {"111", 111}};
                remove_assoc_key(m, "123");
                assert_false(m.count("123"));
                assert_true(m.size() == 1);
                assert_true(m["111"] == 111);
            }

            {
                map<string, int> m{{"123", 123}, {"111", 111}, {"222", 222}};
                remove_assoc_key(m, "123");
                assert_false(m.count("123"));
                assert_true(m.size() == 2);
                assert_true(m["111"] == 111);
                assert_true(m["222"] == 222);
            }

            {
                map<string, int> m{{"123", 123}, {"124", 124}, {"111", 111}, {"222", 222}};
                remove_assoc_key(m, "123", [](const auto& key){ return key == "124"; });
                assert_false(m.count("123"));
                assert_false(m.count("124"));
                assert_true(m.size() == 2);
                assert_true(m["111"] == 111);
                assert_true(m["222"] == 222);
            }

            {
                unordered_multimap<string, int> m{{"123", 123}, {"123", 123}, {"124", 124}, {"124", 124}, {"111", 111}, {"222", 222}};
                remove_assoc_key(m, "123", [](const auto& key){ return key == "124"; });
                assert_false(m.count("123"));
                assert_false(m.count("124"));
                assert_true(m.size() == 2);
                assert_true(m.find("111")->second == 111);
                assert_true(m.find("222")->second == 222);
            }

            {
                unordered_multiset<int> s{1, 2, 3, 4, 4, 4, 4, 5, 5, 5};
                remove_assoc_key(s, 4, [](const auto& key){ return key == 5; });
                assert_false(s.count(4));
                assert_false(s.count(5));
                assert_true(s.size() == 3);
                assert_true(s.count(1));
                assert_true(s.count(2));
                assert_true(s.count(3));
            }
        }

        void remove_assoc_value_test()
        {
            {
                map<string, int> m{{"123", 123}, {"111", 111}};
                remove_assoc_value(m, 123);
                assert_false(m.count("123"));
                assert_true(m.size() == 1);
                assert_true(m["111"] == 111);
            }

            {
                map<string, int> m{{"123", 123}, {"111", 111}, {"222", 222}};
                remove_assoc_value(m, 123);
                assert_false(m.count("123"));
                assert_true(m.size() == 2);
                assert_true(m["111"] == 111);
                assert_true(m["222"] == 222);
            }

            {
                map<string, int> m{{"123", 123}, {"124", 124}, {"111", 111}, {"222", 222}};
                remove_assoc_value(m, 123, [](const auto& value){ return value == 124; });
                assert_false(m.count("123"));
                assert_false(m.count("124"));
                assert_true(m.size() == 2);
                assert_true(m["111"] == 111);
                assert_true(m["222"] == 222);
            }

            {
                unordered_multimap<string, int> m{{"123", 123}, {"123", 123}, {"124", 124}, {"124", 124}, {"111", 111}, {"222", 222}};
                remove_assoc_value(m, 123, [](const auto& value){ return value == 124; });
                assert_false(m.count("123"));
                assert_false(m.count("124"));
                assert_true(m.size() == 2);
                assert_true(m.find("111")->second == 111);
                assert_true(m.find("222")->second == 222);
            }

            {
                unordered_multiset<int> s{1, 2, 3, 4, 4, 4, 4, 5, 5, 5};
                remove_assoc_value(s, 4, [](const auto& value){ return value == 5; });
                assert_false(s.count(4));
                assert_false(s.count(5));
                assert_true(s.size() == 3);
                assert_true(s.count(1));
                assert_true(s.count(2));
                assert_true(s.count(3));
            }
        }

        void remove_assoc_copy_key_test()
        {
            {
                map<string, int> m{{"123", 123}, {"111", 111}};
                auto result = remove_assoc_copy_key(m, "123");
                assert_false(result.count("123"));
                assert_true(result.size() == 1);
                assert_true(result["111"] == 111);
            }

            {
                map<string, int> m{{"123", 123}, {"111", 111}, {"222", 222}};
                auto result = remove_assoc_copy_key(m, "123");
                assert_false(result.count("123"));
                assert_true(result.size() == 2);
                assert_true(result["111"] == 111);
                assert_true(result["222"] == 222);
            }

            {
                map<string, int> m{{"123", 123}, {"124", 124}, {"111", 111}, {"222", 222}};
                auto result = remove_assoc_copy_key(m, "123", [](const auto& key){ return key == "124"; });
                assert_false(result.count("123"));
                assert_false(result.count("124"));
                assert_true(result.size() == 2);
                assert_true(result["111"] == 111);
                assert_true(result["222"] == 222);
            }

            {
                unordered_multimap<string, int> m{{"123", 123}, {"123", 123}, {"124", 124}, {"124", 124}, {"111", 111}, {"222", 222}};
                auto result = remove_assoc_copy_key(m, "123", [](const auto& key){ return key == "124"; });
                assert_false(result.count("123"));
                assert_false(result.count("124"));
                assert_true(result.size() == 2);
                assert_true(result.find("111")->second == 111);
                assert_true(result.find("222")->second == 222);
            }

            {
                unordered_multiset<int> s{1, 2, 3, 4, 4, 4, 4, 5, 5, 5};
                auto result = remove_assoc_copy_key(s, 4, [](const auto& key){ return key == 5; });
                assert_false(result.count(4));
                assert_false(result.count(5));
                assert_true(result.size() == 3);
                assert_true(result.count(1));
                assert_true(result.count(2));
                assert_true(result.count(3));
            }
        }

        void remove_assoc_copy_value_test()
        {
            {
                map<string, int> m{{"123", 123}, {"111", 111}};
                auto result = remove_assoc_copy_value(m, 123);
                assert_false(result.count("123"));
                assert_true(result.size() == 1);
                assert_true(result["111"] == 111);
            }

            {
                map<string, int> m{{"123", 123}, {"111", 111}, {"222", 222}};
                auto result = remove_assoc_copy_value(m, 123);
                assert_false(result.count("123"));
                assert_true(result.size() == 2);
                assert_true(result["111"] == 111);
                assert_true(result["222"] == 222);
            }

            {
                map<string, int> m{{"123", 123}, {"124", 124}, {"111", 111}, {"222", 222}};
                auto result = remove_assoc_copy_value(m, 123, [](const auto& value){ return value == 124; });
                assert_false(result.count("123"));
                assert_false(result.count("124"));
                assert_true(result.size() == 2);
                assert_true(result["111"] == 111);
                assert_true(result["222"] == 222);
            }

            {
                unordered_multimap<string, int> m{{"123", 123}, {"123", 123}, {"124", 124}, {"124", 124}, {"111", 111}, {"222", 222}};
                auto result = remove_assoc_copy_value(m, 123, [](const auto& value){ return value == 124; });
                assert_false(result.count("123"));
                assert_false(result.count("124"));
                assert_true(result.size() == 2);
                assert_true(result.find("111")->second == 111);
                assert_true(result.find("222")->second == 222);
            }

            {
                unordered_multiset<int> s{1, 2, 3, 4, 4, 4, 4, 5, 5, 5};
                auto result = remove_assoc_copy_value(s, 4, [](const auto& value){ return value == 5; });
                assert_false(result.count(4));
                assert_false(result.count(5));
                assert_true(result.size() == 3);
                assert_true(result.count(1));
                assert_true(result.count(2));
                assert_true(result.count(3));
            }
        }

        void run_all_tests()
        {
            satisfies_one_test();
            remove_assoc_key_test();
            remove_assoc_value_test();
            remove_assoc_copy_key_test();
            remove_assoc_copy_value_test();
        }
    }

    void run_all_tests()
    {
        uf::test::meta::run_all_tests();
        uf::test::utils::run_all_tests();

        std::cout << "All tests passed" << std::endl;
    }
}


