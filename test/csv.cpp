#include "testing.hpp"

#include "../useful/csv.hpp"

using namespace uf;
using namespace std::literals;

TEST(csv_raw)
{
    {
        std::string s = "1,2,3\nx,y,z\n";
        std::istringstream stream(s);
        auto result = csv::raw(stream);
        std::vector<std::string> first_row = {"1", "2", "3"};
        std::vector<std::string> second_row = {"x", "y", "z"};
        assert_true(result.size() == 2 && result[0] == first_row && result[1] == second_row);
    }

    {
        std::string s = "1,2,3\r\nx,y,z";
        std::istringstream stream(s);
        auto result = csv::raw(stream);
        std::vector<std::string> first_row = {"1", "2", "3"};
        std::vector<std::string> second_row = {"x", "y", "z"};
        assert_true(result.size() == 2 && result[0] == first_row && result[1] == second_row);
    }

    {
        std::string s = "1,2,3\r\nx,y,z\r\n";
        std::istringstream stream(s);
        auto result = csv::raw(stream);
        std::vector<std::string> first_row = {"1", "2", "3"};
        std::vector<std::string> second_row = {"x", "y", "z"};
        assert_true(result.size() == 2 && result[0] == first_row && result[1] == second_row);
    }

    {
        std::string s = "\"1\",\"\"\"2\",3\r\nx,\"\"\"y\"\"\",\"z\"\r\n";
        std::istringstream stream(s);
        auto result = csv::raw(stream);
        std::vector<std::string> first_row = {"1", "\"2", "3"};
        std::vector<std::string> second_row = {"x", "\"y\"", "z"};
        assert_true(result.size() == 2 && result[0] == first_row && result[1] == second_row);
    }

    {
        try
        {
            std::string s = "\"1\",\"\"\"2\",3\rx,\"\"\"y\"\"\",\"z\"\r\n";
            std::istringstream stream(s);
            auto result = csv::raw(stream);
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
            auto result = csv::raw(stream);
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
            auto result = csv::raw(stream);
        }
        catch (std::runtime_error e)
        {
            assert_eq(std::string(e.what()), "Invalid columns count at line = 2"s);
        }
    }
}
