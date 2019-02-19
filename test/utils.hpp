/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "import.hpp"

namespace test::utils
{
    using namespace uf::utils;

    TEST(satisfies_one)
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
            std::string s = "12345";
            assert_true(satisfies_one(s, "12345"));
            assert_true(satisfies_one(s, [](const std::string& s){ return s == "12345"; }));
            assert_true(satisfies_one(s, [](const std::string&){ return false; }, [](const std::string& s){ return s == "12345"; }));
            assert_false(satisfies_one(s, [](const std::string&){ return false; }));
        }
    }

    TEST(remove_assoc_key)
    {
        {
            std::map<std::string, int> m{{"123", 123}, {"111", 111}};
            remove_assoc_key(m, "123");
            assert_false(m.count("123"));
            assert_true(m.size() == 1);
            assert_true(m["111"] == 111);
        }

        {
            std::map<std::string, int> m{{"123", 123}, {"111", 111}, {"222", 222}};
            remove_assoc_key(m, "123");
            assert_false(m.count("123"));
            assert_true(m.size() == 2);
            assert_true(m["111"] == 111);
            assert_true(m["222"] == 222);
        }

        {
            std::map<std::string, int> m{{"123", 123}, {"124", 124}, {"111", 111}, {"222", 222}};
            remove_assoc_key(m, "123", [](const auto& key){ return key == "124"; });
            assert_false(m.count("123"));
            assert_false(m.count("124"));
            assert_true(m.size() == 2);
            assert_true(m["111"] == 111);
            assert_true(m["222"] == 222);
        }

        {
            std::unordered_multimap<std::string, int> m{{"123", 123}, {"123", 123}, {"124", 124}, {"124", 124}, {"111", 111}, {"222", 222}};
            remove_assoc_key(m, "123", [](const auto& key){ return key == "124"; });
            assert_false(m.count("123"));
            assert_false(m.count("124"));
            assert_true(m.size() == 2);
            assert_true(m.find("111")->second == 111);
            assert_true(m.find("222")->second == 222);
        }

        {
            std::unordered_multiset<int> s{1, 2, 3, 4, 4, 4, 4, 5, 5, 5};
            remove_assoc_key(s, 4, [](const auto& key){ return key == 5; });
            assert_false(s.count(4));
            assert_false(s.count(5));
            assert_true(s.size() == 3);
            assert_true(s.count(1));
            assert_true(s.count(2));
            assert_true(s.count(3));
        }
    }

    TEST(remove_assoc_value)
    {
        {
            std::map<std::string, int> m{{"123", 123}, {"111", 111}};
            remove_assoc_value(m, 123);
            assert_false(m.count("123"));
            assert_true(m.size() == 1);
            assert_true(m["111"] == 111);
        }

        {
            std::map<std::string, int> m{{"123", 123}, {"111", 111}, {"222", 222}};
            remove_assoc_value(m, 123);
            assert_false(m.count("123"));
            assert_true(m.size() == 2);
            assert_true(m["111"] == 111);
            assert_true(m["222"] == 222);
        }

        {
            std::map<std::string, int> m{{"123", 123}, {"124", 124}, {"111", 111}, {"222", 222}};
            remove_assoc_value(m, 123, [](const auto& value){ return value == 124; });
            assert_false(m.count("123"));
            assert_false(m.count("124"));
            assert_true(m.size() == 2);
            assert_true(m["111"] == 111);
            assert_true(m["222"] == 222);
        }

        {
            std::unordered_multimap<std::string, int> m{{"123", 123}, {"123", 123}, {"124", 124}, {"124", 124}, {"111", 111}, {"222", 222}};
            remove_assoc_value(m, 123, [](const auto& value){ return value == 124; });
            assert_false(m.count("123"));
            assert_false(m.count("124"));
            assert_true(m.size() == 2);
            assert_true(m.find("111")->second == 111);
            assert_true(m.find("222")->second == 222);
        }

        {
            std::unordered_multiset<int> s{1, 2, 3, 4, 4, 4, 4, 5, 5, 5};
            remove_assoc_value(s, 4, [](const auto& value){ return value == 5; });
            assert_false(s.count(4));
            assert_false(s.count(5));
            assert_true(s.size() == 3);
            assert_true(s.count(1));
            assert_true(s.count(2));
            assert_true(s.count(3));
        }
    }

    TEST(remove_assoc_copy_key)
    {
        {
            std::map<std::string, int> m{{"123", 123}, {"111", 111}};
            auto result = remove_assoc_copy_key(m, "123");
            assert_false(result.count("123"));
            assert_true(result.size() == 1);
            assert_true(result["111"] == 111);
        }

        {
            std::map<std::string, int> m{{"123", 123}, {"111", 111}, {"222", 222}};
            auto result = remove_assoc_copy_key(m, "123");
            assert_false(result.count("123"));
            assert_true(result.size() == 2);
            assert_true(result["111"] == 111);
            assert_true(result["222"] == 222);
        }

        {
            std::map<std::string, int> m{{"123", 123}, {"124", 124}, {"111", 111}, {"222", 222}};
            auto result = remove_assoc_copy_key(m, "123", [](const auto& key){ return key == "124"; });
            assert_false(result.count("123"));
            assert_false(result.count("124"));
            assert_true(result.size() == 2);
            assert_true(result["111"] == 111);
            assert_true(result["222"] == 222);
        }

        {
            std::unordered_multimap<std::string, int> m{{"123", 123}, {"123", 123}, {"124", 124}, {"124", 124}, {"111", 111}, {"222", 222}};
            auto result = remove_assoc_copy_key(m, "123", [](const auto& key){ return key == "124"; });
            assert_false(result.count("123"));
            assert_false(result.count("124"));
            assert_true(result.size() == 2);
            assert_true(result.find("111")->second == 111);
            assert_true(result.find("222")->second == 222);
        }

        {
            std::unordered_multiset<int> s{1, 2, 3, 4, 4, 4, 4, 5, 5, 5};
            auto result = remove_assoc_copy_key(s, 4, [](const auto& key){ return key == 5; });
            assert_false(result.count(4));
            assert_false(result.count(5));
            assert_true(result.size() == 3);
            assert_true(result.count(1));
            assert_true(result.count(2));
            assert_true(result.count(3));
        }
    }

    TEST(remove_assoc_copy_value)
    {
        {
            std::map<std::string, int> m{{"123", 123}, {"111", 111}};
            auto result = remove_assoc_copy_value(m, 123);
            assert_false(result.count("123"));
            assert_true(result.size() == 1);
            assert_true(result["111"] == 111);
        }

        {
            std::map<std::string, int> m{{"123", 123}, {"111", 111}, {"222", 222}};
            auto result = remove_assoc_copy_value(m, 123);
            assert_false(result.count("123"));
            assert_true(result.size() == 2);
            assert_true(result["111"] == 111);
            assert_true(result["222"] == 222);
        }

        {
            std::map<std::string, int> m{{"123", 123}, {"124", 124}, {"111", 111}, {"222", 222}};
            auto result = remove_assoc_copy_value(m, 123, [](const auto& value){ return value == 124; });
            assert_false(result.count("123"));
            assert_false(result.count("124"));
            assert_true(result.size() == 2);
            assert_true(result["111"] == 111);
            assert_true(result["222"] == 222);
        }

        {
            std::unordered_multimap<std::string, int> m{{"123", 123}, {"123", 123}, {"124", 124}, {"124", 124}, {"111", 111}, {"222", 222}};
            auto result = remove_assoc_copy_value(m, 123, [](const auto& value){ return value == 124; });
            assert_false(result.count("123"));
            assert_false(result.count("124"));
            assert_true(result.size() == 2);
            assert_true(result.find("111")->second == 111);
            assert_true(result.find("222")->second == 222);
        }

        {
            std::unordered_multiset<int> s{1, 2, 3, 4, 4, 4, 4, 5, 5, 5};
            auto result = remove_assoc_copy_value(s, 4, [](const auto& value){ return value == 5; });
            assert_false(result.count(4));
            assert_false(result.count(5));
            assert_true(result.size() == 3);
            assert_true(result.count(1));
            assert_true(result.count(2));
            assert_true(result.count(3));
        }
    }

    TEST(remove_seq)
    {
        {
            std::vector<int> v{1, 2, 3, 4, 5};
            remove_seq(v, 1, 2, 3, 4);
            assert_true(v.size() == 1);
            assert_true(v[0] == 5);
        }

        {
            std::vector<int> v{1, 2, 3, 4, 5, 1};
            remove_seq(v, 1, [](int i){ return i == 2; }, 3, 4);
            assert_true(v.size() == 1);
            assert_true(v[0] == 5);
        }

        {
            std::list<int> l{1, 2, 3, 4, 5, 1};
            remove_seq(l, 1, [](int i){ return i == 2; }, 3, 4);
            assert_true(l.size() == 1);
            assert_true(l.front() == 5);
        }
    }

    TEST(remove_seq_copy)
    {
        {
            std::vector<int> v{1, 2, 3, 4, 5};
            auto result = remove_seq_copy(v, 1, 2, 3, 4);
            assert_true(result.size() == 1);
            assert_true(result[0] == 5);
        }

        {
            std::vector<int> v{1, 2, 3, 4, 5, 1};
            auto result = remove_seq_copy(v, 1, [](int i){ return i == 2; }, 3, 4);
            assert_true(result.size() == 1);
            assert_true(result[0] == 5);
        }

        {
            std::list<int> l{1, 2, 3, 4, 5, 1};
            auto result = remove_seq_copy(l, 1, [](int i){ return i == 2; }, 3, 4);
            assert_true(result.size() == 1);
            assert_true(result.front() == 5);
        }
    }
}
// namespace uf::test::utils
