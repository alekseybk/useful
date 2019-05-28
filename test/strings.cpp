#include "testing.hpp"

#include "../useful/strings.hpp"

using namespace uf;

TEST(split)
{
    {
        std::string s("xx xx     ");
        auto vec = split(s, ' ');
        assert_eq(vec.size(), 2);
        assert_eq(vec[0], std::string("xx"));
        assert_eq(vec[1], std::string("xx"));
    }

    {
        std::string s("x x x       x    ");
        auto vec = split(s, ' ');
        assert_eq(vec.size(), 4);
        for (int i = 0; i < 4; ++i)
            assert_eq(vec[i], std::string("x"));
    }
}

TEST(split_strong)
{
    {
        std::string s("xx xx     ");
        auto vec = split_strong(s, ' ');
        assert_eq(vec.size(), 7);
        assert_eq(vec[0], "xx");
        assert_eq(vec[1], "xx");
        for (int i = 2; i < 7; ++i)
            assert_eq(vec[i], "");
    }

    {
        std::string s("x x x       x    ");
        auto vec = split_strong(s, ' ');
        assert_eq(vec.size(), 14);
        assert_eq(vec[9], "x");
        for (int i = 3; i < 9; ++i)
            assert_eq(vec[i], "");
        for (int i = 10; i < 14; ++i)
            assert_eq(vec[i], "");
    }
}



