// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchQuery.h"

#include "HelperCommon.h"

using namespace std;

SearchQuery::SearchQuery(u16string text, u16string search_dir_path, bool match_case,

                         int size_from, int size_to,

                         bool exclude_hidden_and_system, bool exclude_folders, bool exclude_files,

                         uint c_time_from, uint c_time_to, uint a_time_from, uint a_time_to, uint m_time_from,
                         uint m_time_to)
    :

      Text(text),
      SearchDirPath(search_dir_path),
      MatchCase(match_case),

      SizeFrom(size_from),
      SizeTo(size_to),

      ExcludeHiddenAndSystem(exclude_hidden_and_system),
      ExcludeFolders(exclude_folders),
      ExcludeFiles(exclude_files),

      CTimeFrom(c_time_from),
      CTimeTo(c_time_to),
      ATimeFrom(a_time_from),
      ATimeTo(a_time_to),
      MTimeFrom(m_time_from),
      MTimeTo(m_time_to) {
}

bool operator==(const SearchQuery& lhs, const SearchQuery& rhs) {

    bool res = lhs.Text == rhs.Text && lhs.SearchDirPath == rhs.SearchDirPath && lhs.MatchCase == rhs.MatchCase &&

               lhs.SizeFrom == rhs.SizeFrom && lhs.SizeTo == rhs.SizeTo &&

               lhs.ExcludeHiddenAndSystem == rhs.ExcludeHiddenAndSystem && lhs.ExcludeFolders == rhs.ExcludeFolders &&
               lhs.ExcludeFiles == rhs.ExcludeFiles &&

               lhs.CTimeFrom == rhs.CTimeFrom && lhs.CTimeTo == rhs.CTimeTo && lhs.ATimeFrom == rhs.ATimeFrom &&
               lhs.ATimeTo == rhs.ATimeTo && lhs.MTimeFrom == rhs.MTimeFrom && lhs.MTimeTo == rhs.MTimeTo;

    return res;
}

const wstring g_delim = L"|";

wstring SerializeQuery(const SearchQuery& query) {
    wstring res;

    res += HelperCommon::U16stringToWstring(query.Text) + g_delim;           // 0
    res += HelperCommon::U16stringToWstring(query.SearchDirPath) + g_delim;  // 1
    res += to_wstring(query.MatchCase) + g_delim;   // 2

    res += to_wstring(query.SizeFrom) + g_delim;  // 3
    res += to_wstring(query.SizeTo) + g_delim;    // 4

    res += to_wstring(query.ExcludeHiddenAndSystem) + g_delim;  // 5
    res += to_wstring(query.ExcludeFolders) + g_delim;          // 6
    res += to_wstring(query.ExcludeFiles) + g_delim;            // 7

    res += to_wstring(query.CTimeFrom) + g_delim;  // 8
    res += to_wstring(query.CTimeTo) + g_delim;    // 9
    res += to_wstring(query.ATimeFrom) + g_delim;  // 10
    res += to_wstring(query.ATimeTo) + g_delim;    // 11
    res += to_wstring(query.MTimeFrom) + g_delim;  // 12
    res += to_wstring(query.MTimeTo) + g_delim;    // 13

    return move(res);
}

uSearchQuery DeserializeQuery(const wstring& source) {
    auto parts = HelperCommon::Split(source, g_delim, HelperCommon::SplitOptions::INCLUDE_EMPTY);

    auto text            = HelperCommon::WstringToU16string(parts[0]);
    auto search_dir_path = HelperCommon::WstringToU16string(parts[1]);
    auto match_case      = HelperCommon::ParseNumber<bool>(parts[2]);

    auto size_from = HelperCommon::ParseNumber<int>(parts[3]);
    auto size_to   = HelperCommon::ParseNumber<int>(parts[4]);

    auto exclude_hidden_and_system = HelperCommon::ParseNumber<bool>(parts[5]);
    auto exclude_folders           = HelperCommon::ParseNumber<bool>(parts[6]);
    auto exclude_files             = HelperCommon::ParseNumber<bool>(parts[7]);

    auto c_time_from = HelperCommon::ParseNumber<uint>(parts[8]);
    auto c_time_to   = HelperCommon::ParseNumber<uint>(parts[9]);
    auto a_time_from = HelperCommon::ParseNumber<uint>(parts[10]);
    auto a_time_to   = HelperCommon::ParseNumber<uint>(parts[11]);
    auto m_time_from = HelperCommon::ParseNumber<uint>(parts[12]);
    auto m_time_to   = HelperCommon::ParseNumber<uint>(parts[13]);

    auto query = make_unique<SearchQuery>(text, search_dir_path, match_case, size_from, size_to,
                                          exclude_hidden_and_system, exclude_folders, exclude_files, c_time_from,
                                          c_time_to, a_time_from, a_time_to, m_time_from, m_time_to);

    return query;
}