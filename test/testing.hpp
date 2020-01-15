#pragma once
#include <map>
#include <functional>
#include <cassert>
#include <iostream>

using std::cout;
using std::endl;

#define assert_true(expr) assert(expr)
#define assert_false(expr) assert(!(expr))
#define assert_eq(expr1, expr2) assert((expr1) == (expr2))

std::multimap<std::string, std::function<void()>>& get_test_map() noexcept;

#define TEST(name) \
    static void uf_test_##name(); \
    static int DUMMY_##name = []() noexcept { get_test_map().insert({#name, &uf_test_##name}); return 0; }(); \
    static void uf_test_##name()

