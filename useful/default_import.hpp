/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include <vector>
#include <string>
#include <list>
#include <forward_list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>
#include <deque>

#include <tuple>
#include <type_traits>
#include <functional>
#include <utility>
#include <atomic>
#include <thread>
#include <future>
#include <chrono>
#include <iomanip>

#include "basic_types.hpp"

namespace uf
{
    using namespace std::literals;

    namespace chrono = std::chrono;
    namespace this_thread = std::this_thread;
    namespace placeholders = std::placeholders;

    using std::vector;
    using std::string;
    using std::list;
    using std::forward_list;
    using std::set;
    using std::multiset;
    using std::unordered_set;
    using std::unordered_multiset;
    using std::map;
    using std::multimap;
    using std::unordered_map;
    using std::unordered_multimap;
    using std::stack;
    using std::queue;
    using std::deque;

    using std::tuple;
    using std::pair;
    using std::array;

    using std::stringstream;
    using std::istringstream;
    using std::ostringstream;

    using std::atomic;
    using std::atomic_flag;
    using std::thread;
    using std::future;
    using std::packaged_task;
    using std::promise;
    using std::async;

    using std::exception;
    using std::logic_error;
    using std::runtime_error;

    using std::reference_wrapper;
    using std::numeric_limits;
    using std::function;

    using std::is_same;
    using std::enable_if;
    using std::remove_const;
    using std::remove_cv;
    using std::remove_reference;
    using std::remove_pointer;
    using std::conditional;
    using std::decay;
    using std::tuple_element;

    using std::enable_if_t;
    using std::void_t;
    using std::remove_const_t;
    using std::remove_cv_t;
    using std::remove_reference_t;
    using std::remove_pointer_t;
    using std::conditional_t;
    using std::decay_t;
    using std::tuple_element_t;

    using std::is_lvalue_reference;
    using std::is_reference;
    using std::is_pointer;
    using std::is_const;
    using std::is_same;
    using std::tuple_size;
    using std::is_invocable;
    using std::is_invocable_r;

    using std::is_lvalue_reference_v;
    using std::is_reference_v;
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
// end namesapce uf
