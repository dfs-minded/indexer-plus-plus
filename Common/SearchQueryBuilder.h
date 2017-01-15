// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <limits>
#include <string>

#include "Helpers.h"
#include "SearchQuery.h"
#include "macros.h"

namespace indexer_common {

	class SearchQueryBuilder {
	public:
		SearchQueryBuilder() = default;

		NO_COPY(SearchQueryBuilder)

		~SearchQueryBuilder();

		SearchQueryBuilder& SetSearchText(std::u16string text) { text_ = text; return *this; }
		SearchQueryBuilder& SetSearchDirPath(std::u16string path) { search_dir_path_ = path; return *this; }

		SearchQueryBuilder& SetMatchCase() { match_case_ = true; return *this; }
		SearchQueryBuilder& SetUseRegex() { use_regex_ = true; return *this; }

		SearchQueryBuilder& SetSizeFrom(int val) { size_from_ = val; return *this; }
		SearchQueryBuilder& SetSizeTo(int val) { size_to_ = val; return *this; }

		SearchQueryBuilder& SetExcludeHiddenAndSystem() { exclude_hidden_and_system_ = true; return *this; }

		SearchQueryBuilder& SetExcludeFolders() { exclude_folders_ = true; return *this; }
		SearchQueryBuilder& SetExcludeFiles() { exclude_files_ = true; return *this; }

		SearchQueryBuilder& SetCreationTimeFrom(uint val) { c_time_from_ = val; return *this; }
		SearchQueryBuilder& SetCreationTimeTo(uint val) { c_time_to_ = val; return *this; }
		SearchQueryBuilder& SetLastAccessTimeFrom(uint val) { a_time_from_ = val;  return *this;}
		SearchQueryBuilder& SetLastAccessTimeTo(uint val) { a_time_to_ = val; return *this; }
		SearchQueryBuilder& SetModificationTimeFrom(uint val) { m_time_from_ = val; return *this; }
		SearchQueryBuilder& SetModificationTimeTo(uint val) { m_time_to_ = val;  return *this; }

		uSearchQuery Build() const;

	private:
		std::u16string text_ = Empty16String;
		std::u16string search_dir_path_ = Empty16String;

		bool match_case_ = false;
		bool use_regex_ = false;

		int size_from_ = 0;
		int size_to_ = std::numeric_limits<int>::max();;

		bool exclude_hidden_and_system_ = true;
		bool exclude_folders_ = false;
		bool exclude_files_ = false;

		uint c_time_from_ = std::numeric_limits<uint>::min();
		uint c_time_to_ = std::numeric_limits<uint>::max();
		uint a_time_from_ = std::numeric_limits<uint>::min();
		uint a_time_to_ = std::numeric_limits<uint>::max();
		uint m_time_from_ = std::numeric_limits<uint>::min();
		uint m_time_to_ = std::numeric_limits<uint>::max();
	};


	std::wstring SerializeQuery(const SearchQuery& query);

	uSearchQuery DeserializeQuery(const std::wstring& source);

} //namespace indexer_common