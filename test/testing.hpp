/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include <map>
#include <functional>
#include <cassert>

#define assert_true(expr) assert(expr)
#define assert_false(expr) assert(!(expr))
#define assert_eq(expr1, expr2) assert((expr1) == (expr2))

std::multimap<std::string, std::function<void()>>& get_test_map();

#define TEST(name) \
    static void uf_test_##name(); \
    static int DUMMY_##name = (get_test_map().insert({#name, &uf_test_##name}), 1); \
    static void uf_test_##name()

