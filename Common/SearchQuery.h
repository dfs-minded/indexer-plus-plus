// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <functional>
#include <memory>
#include <string>

#include "macros.h"
#include "typedefs.h"

namespace indexer_common {

	class SearchQueryBuilder;

	struct SearchQuery {
	public:
		friend class SearchQueryBuilder; // Calls private ctor and dtor of this class.

		NO_COPY(SearchQuery)

		~SearchQuery();

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

	private:

		// Constructs an immutable query.
		SearchQuery(std::u16string text, std::u16string search_dir_path,
			bool match_case, bool use_regex,
			int size_from, int size_to,
			bool exclude_hidden_and_system, bool exclude_folders, bool exclude_files,
			uint c_time_from, uint c_time_to, uint a_time_from, uint a_time_to, uint m_time_from, uint m_time_to);
	};


	using uSearchQuery = std::unique_ptr<SearchQuery, std::function<void(SearchQuery*)>>;

	bool operator==(const SearchQuery& lhs, const SearchQuery& rhs);

}  // namespace indexer_common