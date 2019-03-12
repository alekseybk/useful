/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "utils.hpp"
#include "streams.hpp"
#include "strings.hpp"

namespace uf::csv
{
    namespace detail
    {
        std::pair<std::string_view, std::string> cell_escaped(std::string_view sv, text_pos& pos)
        {
            if (sv[0] != '\"')
                throw std::runtime_error("Escaped csv cell must begin with \" at line = " + std::to_string(pos.line) + " col = " + std::to_string(pos.col));
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
            throw std::runtime_error("Escaped csv cell must ends with \" at line = " + std::to_string(pos.line) + " col = " + std::to_string(pos.col));
        }

        std::pair<std::string_view, std::string> cell_non_escaped(std::string_view sv, text_pos& pos)
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

        std::pair<std::string_view, std::vector<std::string>> record(std::string_view sv, text_pos& pos)
        {
            std::vector<std::string> result;
            while (true)
            {
                std::pair<std::string_view, std::string> cell_out;
                if (sv[0] == '\"')
                    cell_out = cell_escaped(sv, pos);
                else
                    cell_out = cell_non_escaped(sv, pos);
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
                            throw std::runtime_error("Expected \\n after \\r at line = " + std::to_string(pos.line) + " col = " + std::to_string(pos.col));
                        pos.inc(sv[1]);
                        return {std::string_view(sv.data() + 2, sv.size() - 2), std::move(result)};
                }
            }
        }
    }
    // namespace detail

    // RFC 4180
    template<class InputStream>
    std::vector<std::vector<std::string>> raw(InputStream&& stream) noexcept(false)
    {
        std::string data = read_unf_str(stream, bytes_remaining(stream));
        std::string_view sv(data);
        std::vector<std::vector<std::string>> result;
        text_pos pos;
        auto push_row = [&result, &sv, &pos]()
        {
            auto rec_res = detail::record(sv, pos);
            result.push_back(std::move(rec_res.second));
            sv = rec_res.first;
        };
        push_row();

        const u64 cols = result.back().size();
        while (sv.size())
        {
            push_row();
            if (result.back().size() != cols)
                throw std::runtime_error("Invalid columns count at line = " + std::to_string(pos.line));
        }
        return result;
    }
}
// namespace uf

