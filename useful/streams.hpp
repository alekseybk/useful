#pragma once
#include "utils.hpp"

namespace uf
{
    struct text_pos
    {
        u64 line = 1;
        u64 col = 1;

        void inc(char c) noexcept
        {
            if (c == '\n')
            {
                ++line;
                col = 1;
            }
            else
                ++col;
        }
    };

    template<class InputStream>
    u64 bytes_remaining(InputStream&& stream)
    {
        const u64 cur_pos = stream.tellg();
        stream.seekg(0, stream.end);
        const u64 result = stream.tellg() - static_cast<i64>(cur_pos);
        stream.seekg(cur_pos);
        return result;
    }

    template<class Tp, class InputStream>
    std::pair<u64, u64> elements_remaining(InputStream&& stream)
    {
        const u64 bytes = bytes_remaining(std::forward<InputStream>(stream));
        return {bytes % sizeof(Tp), bytes / sizeof(Tp)};
    }

    template<typename Tp, class InputStream>
    void read_unf_val(InputStream&& stream, Tp& value)
    {
        stream.read(reinterpret_cast<char*>(&value), sizeof(Tp));
    }

    template<typename Tp, class InputStream>
    Tp read_unf_val(InputStream&& stream)
    {
        Tp result;
        read_unf_val(stream, result);
        return result;
    }

    template<class InputStream>
    u64 read_unf_data(InputStream&& stream, void* data, u64 bytes)
    {
        stream.read(static_cast<char*>(data), bytes);
        return stream.gcount();
    }

    template<class InputStream, typename Tp>
    u64 read_unf_data(InputStream&& stream, Tp* data, u64 count)
    {
        stream.read(reinterpret_cast<char*>(data), count * sizeof(Tp));
        const u64 readed = stream.gcount();
        const u64 remaining = readed % sizeof(Tp);
        if (remaining)
            throw std::runtime_error(std::string("Stream extra bytes = ") + std::to_string(remaining));
        return readed / sizeof(Tp);
    }

    template<class InputStream, class SmartPtr, enif<mt::is_smart_pointer_v<SmartPtr>> = sdef>
    u64 read_unf_data(InputStream&& stream, SmartPtr data, u64 count)
    {
        return read_unf_data(std::forward<InputStream>(stream), data.get(), count);
    }

    template<class Container, class InputStream>
    Container read_unf_container(InputStream&& stream, u64 count)
    {
        Container result;
        result.resize(count);
        const u64 readed = read_unf_data(stream, result.data(), count);
        result.resize(readed);
        result.shrink_to_fit();
        return result;
    }

    template<typename Tp, class InputStream>
    std::vector<Tp> read_unf_vec(InputStream&& stream, u64 count)
    {
        return read_unf_container<std::vector<Tp>>(stream, count);
    }

    template<class InputStream>
    std::string read_unf_str(InputStream&& stream, u64 count)
    {
        return read_unf_container<std::string>(stream, count);
    }
}
// namespace uf
