/*
 * Copyright Aleksey Verkholat 2018
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt
*/

#pragma once
#include "default_import.hpp"

namespace uf::formats
{
    template<class Stream>
    vector<vector<string>> csv_trivial(Stream&& stream, char delimiter = ',') noexcept(false)
    {
        u64 cols = 0;
        vector<vector<string>> result;

        string record;
        vector<string> line;
        bool qstate = false;

        string s;
        while (std::getline(stream, s))
        {
            for (u64 i = 0; i < s.size(); ++i)
            {
                if (qstate)
                {
                    if (s[i] == '\"')
                    {
                        if (i != s.size() - 1 && s[i + 1] == '\"')
                            ++i;
                        else
                        {
                            qstate = false;
                            continue;
                        }
                    }
                }
                else
                {
                    if (s[i] == delimiter)
                    {
                        line.push_back(std::move(record));
                        continue;
                    }
                    if (s[i] == '\"')
                    {
                        qstate = true;
                        continue;
                    }
                }
                record.push_back(s[i]);
            }
            if (!qstate)
            {
                line.push_back(std::move(record));
                if (cols == 0)
                    cols = line.size();
                if (line.size() != cols)
                    throw runtime_error("invalid line "s + to_string(result.size()) +
                                        ", size = " + to_string(line.size()) + ", expected = " + to_string(cols));
                result.push_back(std::move(line));
            }
        }
        return result;
    }
}
// namespace uf::formats
