#include "testing.hpp"

#include "../useful/streams.hpp"

using namespace uf;

TEST(bytes_remaining)
{
    {
        std::istringstream stream("");
        assert_eq(bytes_remaining(stream), 0);
    }

    {
        std::istringstream stream("123");
        assert_eq(bytes_remaining(stream), 3);
    }

    {
        std::istringstream stream("123");
        char a;
        stream.read(&a, 1);
        assert_eq(bytes_remaining(stream), 2);
    }
}

TEST(read_unf_value)
{
    {
        std::istringstream stream("\xFF\xFF\xFF\xFF");
        u32 x = read_unf_val<u32>(stream);
        assert_eq(x, std::numeric_limits<u32>::max());
    }
}

TEST(read_unf_data)
{
    {
        std::string s("123");
        std::istringstream stream(s);
        std::shared_ptr<u8[]> data(new u8[3]);
        read_unf_data(stream, data.get(), 3);
        assert_false(memcmp(s.data(), data.get(), s.size()));
        read_unf_data(stream, data, 3);
        assert_false(memcmp(s.data(), data.get(), s.size()));
    }

    {
        std::string s("123");
        std::istringstream stream(s);
        std::unique_ptr<u16[]> data(new u16[2]);
        try
        {
            read_unf_data(stream, data.get(), 2);
        }
        catch (std::runtime_error e)
        {
            assert_false(strcmp(e.what(), "Stream extra bytes = 1"));
            return;
        }
        assert(false);
    }
}

TEST(read_unf_vec)
{
    {
        std::string s = "123223";
        std::istringstream stream(s);
        auto result = read_unf_vec<u16>(stream, bytes_remaining(stream));
        assert_false(memcmp(s.data(), result.data(), s.size()));
    }
}

TEST(read_unf_str)
{
    {
        std::string s = "123223gf0dg0\n\r\tidfhg0idf\n\ng";
        std::istringstream stream(s);
        std::string s2 = read_unf_str(stream, bytes_remaining(stream));
        assert_eq(s, s2);
    }
}
