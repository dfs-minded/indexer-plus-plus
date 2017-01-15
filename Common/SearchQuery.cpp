// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchQuery.h"

namespace indexer_common {

	using std::u16string;
	using std::wstring;
	using std::to_wstring;

	SearchQuery::SearchQuery(u16string text, u16string search_dir_path,
		bool match_case, bool use_regex,
		int size_from, int size_to,
		bool exclude_hidden_and_system, bool exclude_folders, bool exclude_files,
		uint c_time_from, uint c_time_to, uint a_time_from, uint a_time_to, uint m_time_from, uint m_time_to)
		:
		Text(move(text)),
		SearchDirPath(move(search_dir_path)),
		MatchCase(match_case),
		UseRegex(use_regex),

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

	SearchQuery::~SearchQuery() = default;

	bool operator==(const SearchQuery& lhs, const SearchQuery& rhs) {

		bool res = lhs.Text == rhs.Text && lhs.SearchDirPath == rhs.SearchDirPath && lhs.MatchCase == rhs.MatchCase &&

			lhs.SizeFrom == rhs.SizeFrom && lhs.SizeTo == rhs.SizeTo &&

			lhs.ExcludeHiddenAndSystem == rhs.ExcludeHiddenAndSystem && lhs.ExcludeFolders == rhs.ExcludeFolders &&
			lhs.ExcludeFiles == rhs.ExcludeFiles &&

			lhs.CTimeFrom == rhs.CTimeFrom && lhs.CTimeTo == rhs.CTimeTo && lhs.ATimeFrom == rhs.ATimeFrom &&
			lhs.ATimeTo == rhs.ATimeTo && lhs.MTimeFrom == rhs.MTimeFrom && lhs.MTimeTo == rhs.MTimeTo;

		return res;
	}

}  // namespace indexer_common