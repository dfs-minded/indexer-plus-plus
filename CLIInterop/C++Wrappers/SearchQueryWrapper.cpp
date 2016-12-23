// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchQueryWrapper.h"

#include <climits>

#include "InteropHelper.h"

// clang-format off

namespace CLIInterop {
	
	SearchQueryWrapper::SearchQueryWrapper() 
		: Text(""), SearchDirPath("")
	{
	}

	indexer_common::SearchQuery* SearchQueryWrapper::ToUnmanagedQuery()
	{
		// Adjusting to the end of the day.
		CreatedTimeTo = CreatedTimeTo->AddDays(1).Subtract(System::TimeSpan::FromMilliseconds(1));

		auto text = Helper::ToU16string(Text);
		auto searchDirPath = Helper::ToU16string(SearchDirPath);

		int sizeFrom = SizeFrom;
		int sizeTo = SizeTo != 0 ? SizeTo : INT32_MAX;

		indexer_common::uint cTimeFrom =
			(CreatedTimeFrom->Year < 1970) ? 0 : Helper::DateTimeToUnixTimeSeconds(CreatedTimeFrom);

		indexer_common::uint cTimeTo =
			(CreatedTimeTo->Year < 1970) ? UINT32_MAX : Helper::DateTimeToUnixTimeSeconds(CreatedTimeTo);

		auto query = new indexer_common::SearchQuery(move(text), searchDirPath, MatchCase, UseRegex, sizeFrom, sizeTo,
			ExcludeHiddenAndSystem, ExcludeFolders, ExcludeFiles, cTimeFrom, cTimeTo);

		return query;
	}
}