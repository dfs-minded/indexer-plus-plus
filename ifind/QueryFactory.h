// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <tchar.h>
#include <map>
#include <string>
#include <vector>

#include "IndexerDateTime.h"
#include "Macros.h"
#include "SearchQuery.h"

namespace ifind {

	class QueryFactory {

	   public:
		QueryFactory() = default;

		NO_COPY(QueryFactory)

		~QueryFactory() = default;

		indexer_common::uSearchQuery ParseInput(const std::vector<std::wstring>& args, std::wstring* format, std::wstring* outputPath);

	   private:
		void SetType(const std::wstring& type, bool* excludeFiles, bool* excludeFolders);

		void SetSize(const std::wstring& sizeText, int* sizeFrom, int* sizeTo);

		int TryParseSize(const std::wstring& text);

		void SetTime(const std::wstring& timeText, indexer_common::uint* timeFrom, indexer_common::uint* timeTo, indexer_common::DateTimeEnum timeType);

		// TODO: remove std::map at right-hand side after stopping supporing VS 2013.
		std::map<wchar_t, int> multipliers = std::map<wchar_t, int>({{L'k', 1}, {L'M', 1024}, {L'G', 1024 * 1024}});
	};

} // namespace ifind