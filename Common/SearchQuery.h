// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <climits>
#include <memory>
#include <string>

#include "Helpers.h"
#include "Macros.h"
#include "typedefs.h"

namespace indexer_common {

    struct SearchQuery {
       public:
        NO_COPY(SearchQuery)

        // Constructs immutable query.
        SearchQuery(std::u16string text = Empty16String, std::u16string search_dir_path = Empty16String,
                    bool match_case = false, bool use_regex = false,

                    int size_from = 0, int size_to = INT_MAX,

                    bool exclude_hidden_and_system = true, bool exclude_folders = false, bool exclude_files = false,

                    uint c_time_from = 0, uint c_time_to = UINT_MAX, uint a_time_from = 0, uint a_time_to = UINT_MAX,
                    uint m_time_from = 0, uint m_time_to = UINT_MAX);


        const std::u16string Text;
        const std::u16string SearchDirPath;
        const bool MatchCase;
        const bool UseRegex;

        const int SizeFrom;
        const int SizeTo;

        const bool ExcludeHiddenAndSystem;
        const bool ExcludeFolders;
        const bool ExcludeFiles;

        // Creation time filters.
        const uint CTimeFrom;
        const uint CTimeTo;

        // Last accessed time filters.
        const uint ATimeFrom;
        const uint ATimeTo;

        // Last modification time filters.
        const uint MTimeFrom;
        const uint MTimeTo;
    };

    typedef std::unique_ptr<SearchQuery> uSearchQuery;

    bool operator==(const SearchQuery& lhs, const SearchQuery& rhs);

    std::wstring SerializeQuery(const SearchQuery& query);

    uSearchQuery DeserializeQuery(const std::wstring& source);

} // namespace indexer_common