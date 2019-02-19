/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "../useful/all.hpp"

#include <cassert>
#include <vector>
#include <functional>

#define assert_true(expr) assert(expr)
#define assert_false(expr) assert(!(expr))
#define assert_eq(expr1, expr2) assert((expr1) == (expr2))

#define TEST(name) \
    void uf_test_##name(); \
    int DUMMY_##name = (get_test_vector().push_back(&uf_test_##name), 1); \
    void uf_test_##name()

std::vector<std::function<void()>>& get_test_vector();

using namespace uf::ints;
using namespace uf::literals;
using namespace std::literals;
