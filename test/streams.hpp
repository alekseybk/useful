/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "import.hpp"

namespace test::streams
{
    using namespace uf::streams;

    TEST(stream_bytes_remaining)
    {
        {
            std::istringstream stream("");
            assert_eq(stream_bytes_remaining(stream), 0);
        }

        {
            std::istringstream stream("123");
            assert_eq(stream_bytes_remaining(stream), 3);
        }

        {
            std::istringstream stream("123");
            char a;
            stream.read(&a, 1);
            assert_eq(stream_bytes_remaining(stream), 2);
        }
    }

    TEST(read_stream_to_vec)
    {
        {
            std::string s = "123223";
            std::istringstream stream(s);
            auto bytes = read_stream_to_vec<byte>(stream);
            assert_false(memcmp(s.data(), bytes.data(), s.size()));
        }

        {
            std::string s = "123223";
            std::istringstream stream(s);
            auto [rem, result] = read_stream_to_vec<u16>(stream);
            assert_eq(rem, 0);
            assert_false(memcmp(s.data(), result.data(), s.size()));
        }

        {
            std::string s = "1232231";
            std::istringstream stream(s);
            auto [rem, result] = read_stream_to_vec<u16>(stream);
            assert_eq(rem, 1);
            assert_false(memcmp(s.data(), result.data(), s.size() - 1));
        }
    }

    TEST(read_stream_to_str)
    {
        {
            std::string s = "123223gf0dg0\n\r\tidfhg0idf\n\ng";
            std::istringstream stream(s);
            std::string s2 = read_stream_to_str(stream);
            assert_eq(s, s2);
        }

        {
            std::string s = "123\n456\r789\t\n";
            std::stringstream stream;
            stream << s << std::endl;
            std::string s2 = read_stream_to_str(stream);
            assert_eq(s + "\n", s2);
        }
    }

    TEST(csv_raw)
    {
        {
            std::string s = "1,2,3\nx,y,z\n";
            std::istringstream stream(s);
            auto result = csv_raw(stream);
            std::vector<std::string> first_row = {"1", "2", "3"};
            std::vector<std::string> second_row = {"x", "y", "z"};
            assert_true(result.size() == 2 && result[0] == first_row && result[1] == second_row);
        }

        {
            std::string s = "1,2,3\r\nx,y,z";
            std::istringstream stream(s);
            auto result = csv_raw(stream);
            std::vector<std::string> first_row = {"1", "2", "3"};
            std::vector<std::string> second_row = {"x", "y", "z"};
            assert_true(result.size() == 2 && result[0] == first_row && result[1] == second_row);
        }

        {
            std::string s = "1,2,3\r\nx,y,z\r\n";
            std::istringstream stream(s);
            auto result = csv_raw(stream);
            std::vector<std::string> first_row = {"1", "2", "3"};
            std::vector<std::string> second_row = {"x", "y", "z"};
            assert_true(result.size() == 2 && result[0] == first_row && result[1] == second_row);
        }

        {
            std::string s = "\"1\",\"\"\"2\",3\r\nx,\"\"\"y\"\"\",\"z\"\r\n";
            std::istringstream stream(s);
            auto result = csv_raw(stream);
            std::vector<std::string> first_row = {"1", "\"2", "3"};
            std::vector<std::string> second_row = {"x", "\"y\"", "z"};
            assert_true(result.size() == 2 && result[0] == first_row && result[1] == second_row);
        }

        {
            try
            {
                std::string s = "\"1\",\"\"\"2\",3\rx,\"\"\"y\"\"\",\"z\"\r\n";
                std::istringstream stream(s);
                auto result = csv_raw(stream);
            }
            catch (std::runtime_error e)
            {
                assert_eq(std::string(e.what()), "Expected \\n after \\r at line = 1 col = 13");
            }
        }

        {
            try
            {
                std::string s = "\"1\",\"\"\"2\",3\r\nx,\"\"\"y\"\"\",\"z";
                std::istringstream stream(s);
                auto result = csv_raw(stream);
            }
            catch (std::runtime_error e)
            {
                assert_eq(std::string(e.what()), "Escaped csv cell must ends with \" at line = 2 col = 13"s);
            }
        }

        {
            try
            {
                std::string s = "\"1\",\"\"\"2\",3\r\nx,\"z\"";
                std::istringstream stream(s);
                auto result = csv_raw(stream);
            }
            catch (std::runtime_error e)
            {
                assert_eq(std::string(e.what()), "Invalid columns count at line = 2"s);
            }
        }
    }
}
// namespace uf::test::streams
