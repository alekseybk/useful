/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <tuple>
#include <type_traits>
#include <functional>
#include <utility>
#include <atomic>
#include <thread>
#include <chrono>
#include <iomanip>

#include "short_int.hpp"

namespace uf
{
    using namespace short_int;
    using namespace short_int::literals;

    using namespace std::literals;
    using namespace std::chrono_literals;

    namespace chrono = std::chrono;
    namespace this_thread = std::this_thread;

    using std::vector;
    using std::string;
    using std::map;
    using std::unordered_map;
    using std::tuple;
    using std::pair;
    using std::reference_wrapper;
    using std::function;
    using std::atomic;
    using std::atomic_flag;
    using std::stringstream;
    using std::runtime_error;

    using std::is_same;

    using std::enable_if_t;
    using std::void_t;
    using std::remove_const_t;
    using std::remove_cv_t;
    using std::remove_reference_t;
    using std::remove_pointer_t;
    using std::conditional_t;
    using std::decay_t;
    using std::tuple_element_t;

    using std::is_lvalue_reference_v;
    using std::is_pointer_v;
    using std::is_const_v;
    using std::is_same_v;
    using std::tuple_size_v;
    using std::is_invocable_v;
    using std::is_invocable_r_v;

    using std::true_type;
    using std::false_type;
    using std::index_sequence;
    using std::make_index_sequence;
    using std::integral_constant;

    using std::declval;
    using std::to_string;
}
// end namesapce useful
