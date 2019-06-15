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

    {
        std::string s("xx xx     ");
        auto vec = split_n(s, 1, ' ');
        assert_eq(vec.size(), 1);
        assert_eq(vec[0], std::string("xx"));
    }
}



