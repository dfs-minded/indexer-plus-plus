// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchQueryBuilder.h"

namespace indexer_common {

	using std::wstring;
	using std::to_wstring;

	SearchQueryBuilder::~SearchQueryBuilder() = default;

	namespace {

	const wstring kDelim = L"|";

	} // namespace

	wstring SerializeQuery(const SearchQuery& query) {
		wstring res;

		res += helpers::U16stringToWstring(query.Text) + kDelim;           // 0
		res += helpers::U16stringToWstring(query.SearchDirPath) + kDelim;  // 1
		res += to_wstring(query.MatchCase) + kDelim;                       // 2
		res += to_wstring(query.UseRegex) + kDelim;                        // 3

		res += to_wstring(query.SizeFrom) + kDelim;  // 4
		res += to_wstring(query.SizeTo) + kDelim;    // 5

		res += to_wstring(query.ExcludeHiddenAndSystem) + kDelim;  // 6
		res += to_wstring(query.ExcludeFolders) + kDelim;          // 7
		res += to_wstring(query.ExcludeFiles) + kDelim;            // 8

		res += to_wstring(query.CTimeFrom) + kDelim;  // 9
		res += to_wstring(query.CTimeTo) + kDelim;    // 10
		res += to_wstring(query.ATimeFrom) + kDelim;  // 11
		res += to_wstring(query.ATimeTo) + kDelim;    // 12
		res += to_wstring(query.MTimeFrom) + kDelim;  // 13
		res += to_wstring(query.MTimeTo) + kDelim;    // 14

		return std::move(res);
	}

	uSearchQuery DeserializeQuery(const wstring& source) {
		auto parts = helpers::Split(source, kDelim, helpers::SplitOptions::INCLUDE_EMPTY);

		SearchQueryBuilder builder;

		auto text = helpers::WstringToU16string(parts[0]);
		auto search_dir_path = helpers::WstringToU16string(parts[1]);
		builder.SetSearchText(text).SetSearchDirPath(search_dir_path);

		auto match_case = helpers::ParseNumber<bool>(parts[2]);
		if (match_case) builder.SetMatchCase();
		auto use_regex = helpers::ParseNumber<bool>(parts[3]);
		if (use_regex) builder.SetUseRegex();

		auto size_from = helpers::ParseNumber<int>(parts[4]);
		auto size_to = helpers::ParseNumber<int>(parts[5]);
		builder.SetSizeFrom(size_from).SetSizeTo(size_to);

		auto exclude_hidden = helpers::ParseNumber<bool>(parts[6]);
		if (exclude_hidden) builder.SetExcludeHiddenAndSystem();
		auto exclude_folders = helpers::ParseNumber<bool>(parts[7]);
		if (exclude_folders) builder.SetExcludeFolders();
		auto exclude_files = helpers::ParseNumber<bool>(parts[8]);
		if (exclude_files) builder.SetExcludeFiles();

		auto c_time_from = helpers::ParseNumber<uint>(parts[9]);
		auto c_time_to = helpers::ParseNumber<uint>(parts[10]);
		auto a_time_from = helpers::ParseNumber<uint>(parts[11]);
		auto a_time_to = helpers::ParseNumber<uint>(parts[12]);
		auto m_time_from = helpers::ParseNumber<uint>(parts[13]);
		auto m_time_to = helpers::ParseNumber<uint>(parts[14]);

		builder.SetCreationTimeFrom(c_time_from).SetCreationTimeTo(c_time_to)
			   .SetLastAccessTimeFrom(a_time_from).SetLastAccessTimeTo(a_time_to)
			   .SetModificationTimeFrom(m_time_from).SetModificationTimeTo(m_time_to);

		return builder.Build();
	}

	uSearchQuery SearchQueryBuilder::Build() const {
		
		return std::unique_ptr<SearchQuery>(new SearchQuery(
				text_, search_dir_path_,
				match_case_, use_regex_,
				size_from_, size_to_,
				exclude_hidden_and_system_, exclude_folders_, exclude_files_,
				c_time_from_, c_time_to_, a_time_from_, a_time_to_, m_time_from_, m_time_to_));
	}

} // namespace indexer_common