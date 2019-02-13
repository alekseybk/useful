/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "utils.hpp"

namespace uf::streams
{
    class text_file_pos
    {
        u64 line_ = 1;
        u64 col_ = 1;

    public:
        void inc(char c) noexcept
        {
            if (c == '\n')
            {
                ++line_;
                col_ = 1;
            }
            else
                ++col_;
        }

        void inc_col() noexcept
        {
            ++col_;
        }

        void inc_line() noexcept
        {
            ++line_;
        }

        u64 line() const noexcept
        {
            return line_;
        }

        u64 col() const noexcept
        {
            return col_;
        }
    };

    namespace detail
    {
        std::pair<std::string_view, std::string> csv_cell_escaped(std::string_view sv, text_file_pos& pos)
        {
            if (sv[0] != '\"')
                throw std::runtime_error("Escaped csv cell must begin with \" at line = " + std::to_string(pos.line()) + " col = " + std::to_string(pos.col()));
            pos.inc(sv[0]);
            std::string result;
            for (u64 i = 1; i < sv.size(); ++i)
            {
                pos.inc(sv[i]);
                if (sv[i] == '\"')
                {
                    if (i != sv.size() - 1 && sv[i + 1] == '\"')
                    {
                        ++i;
                        pos.inc(sv[i + 1]);
                        result.push_back('\"');
                    }
                    else
                        return {std::string_view(sv.data() + i + 1, sv.size() - i - 1), std::move(result)};
                }
                else
                    result.push_back(sv[i]);
            }
            throw std::runtime_error("Escaped csv cell must ends with \" at line = " + std::to_string(pos.line()) + " col = " + std::to_string(pos.col()));
        }

        std::pair<std::string_view, std::string> csv_cell_non_escaped(std::string_view sv, text_file_pos& pos)
        {
            u64 i = 0;
            for (; i < sv.size(); ++i)
            {
                if (sv[i] == ',' || sv[i] == '\n' || sv[i] == '\r')
                    break;
                pos.inc(sv[i]);
            }
            return {std::string_view(sv.data() + i, sv.size() - i), std::string(sv.begin(), sv.begin() + i)};
        }

        std::pair<std::string_view, std::vector<std::string>> csv_record(std::string_view sv, text_file_pos& pos)
        {
            std::vector<std::string> result;
            while (true)
            {
                std::pair<std::string_view, std::string> cell_out;
                if (sv[0] == '\"')
                    cell_out = csv_cell_escaped(sv, pos);
                else
                    cell_out = csv_cell_non_escaped(sv, pos);
                sv = cell_out.first;
                result.push_back(std::move(cell_out.second));

                if (!sv.size())
                    return {std::string_view(), std::move(result)};
                pos.inc(sv[0]);
                switch (sv[0])
                {
                    case '\n':
                        return {std::string_view(sv.data() + 1, sv.size() - 1), std::move(result)};
                    case ',':
                        sv = std::string_view(sv.data() + 1, sv.size() - 1);
                        continue;
                    case '\r':
                        if (!sv.size() || sv[1] != '\n')
                            throw std::runtime_error("Expected \\n after \\r at line = " + std::to_string(pos.line()) + " col = " + std::to_string(pos.col()));
                        pos.inc(sv[1]);
                        return {std::string_view(sv.data() + 2, sv.size() - 2), std::move(result)};
                }
            }
        }
    }
    // namespace detail

    template<class InputStream>
    u64 stream_bytes_remaining(InputStream&& stream)
    {
        const u64 cur_pos = stream.tellg();
        stream.seekg(0, stream.end);
        const u64 result = stream.tellg() - static_cast<i64>(cur_pos);
        stream.seekg(cur_pos);
        return result;
    }

    template<typename Tp, class InputStream>
    auto read_stream_to_vec(InputStream&& stream, u64 bytes = std::numeric_limits<u64>::max())
    {
        const u64 size = stream_bytes_remaining(stream);
        if (size < bytes)
            bytes = size;
        const u64 rem = bytes % sizeof(Tp);
        bytes -= rem;
        std::vector<Tp> result(bytes / sizeof(Tp));
        stream.read(reinterpret_cast<char*>(result.data()), bytes);
        if constexpr (sizeof(Tp) > 1)
            return std::pair(rem, std::move(result));
        else
            return result;
    }

    template<class InputStream>
    std::string read_stream_to_str(InputStream&& stream, u64 bytes = std::numeric_limits<u64>::max())
    {
        const u64 size = stream_bytes_remaining(stream);
        if (size < bytes)
            bytes = size;
        std::string result;
        result.resize(bytes);
        stream.read(result.data(), bytes);
        return result;
    }

    // RFC 4180
    template<class InputStream>
    std::vector<std::vector<std::string>> csv_raw(InputStream&& stream) noexcept(false)
    {
        std::string data = read_stream_to_str(stream);

        std::string_view sv(data);
        std::vector<std::vector<std::string>> result;
        text_file_pos pos;
        auto push_row = [&result, &sv, &pos]()
        {
            auto rec_res = detail::csv_record(sv, pos);
            result.push_back(std::move(rec_res.second));
            sv = rec_res.first;
        };
        push_row();

        const u64 cols = result.back().size();
        while (sv.size())
        {
            push_row();
            if (result.back().size() != cols)
                throw std::runtime_error("Invalid columns count at line = " + std::to_string(pos.line()));
        }
        return result;
    }
}
// namespace uf::streams

namespace uf
{
    inline namespace operators
    {
        inline namespace streams_operators
        {
            inline namespace input
            {
                template<typename... StreamArgs, typename... Ts>
                auto& operator>>(std::basic_istream<StreamArgs...>& stream, std::tuple<Ts...>& t)
                {
                    utils::tuple_for_each(t, [&stream](auto& e){ stream >> e; });
                    return stream;
                }

                template<typename... StreamArgs, typename F, typename S>
                auto& operator>>(std::basic_istream<StreamArgs...>& stream, std::pair<F, S>& p)
                {
                    return stream >> p.first >> p.second;
                }
            }
            // namespace input

            inline namespace output
            {
                template<typename... StreamArgs, typename... Ts>
                auto& operator<<(std::basic_ostream<StreamArgs...>& stream, const std::tuple<Ts...>& t)
                {
                    utils::tuple_for_each<sizeof...(Ts) - 1>(t, [&stream](const auto& e){ stream << e << " "; });
                    return stream << std::get<sizeof...(Ts) - 1>(t);
                }

                template<typename... StreamArgs, class F, class S>
                auto& operator<<(std::basic_ostream<StreamArgs...>& stream, const std::pair<F, S>& p)
                {
                    return stream << p.first << " " << p.second;
                }
            }
            // namespace output
        }
        // namespace streams
    }
    // namespace operators
}
// namespace uf
