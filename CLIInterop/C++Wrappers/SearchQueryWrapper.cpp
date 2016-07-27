// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchQueryWrapper.h"

#include <climits>

#include "InteropHelper.h"

using namespace System;
using namespace std;
// clang-format off

namespace CLIInterop
{
	SearchQueryWrapper::SearchQueryWrapper() : 
		Text(""),
		SearchDirPath("")
	{
	}

	SearchQuery* SearchQueryWrapper::ToUnmanagedQuery()
	{
		// Adjusting to the end of the day.
		CreatedTimeTo = CreatedTimeTo->AddDays(1).Subtract(TimeSpan::FromMilliseconds(1));

		auto text = InteropHelper::ToU16string(Text);
		auto searchDirPath = InteropHelper::ToU16string(SearchDirPath);

		int sizeFrom = SizeFrom;
		int sizeTo = SizeTo != 0 ? SizeTo : INT32_MAX;

		uint cTimeFrom =
			(CreatedTimeFrom->Year < 1970) ? 0 : InteropHelper::DateTimeToUnixTimeSeconds(CreatedTimeFrom);

		uint cTimeTo =
			(CreatedTimeTo->Year < 1970) ? UINT32_MAX : InteropHelper::DateTimeToUnixTimeSeconds(CreatedTimeTo);

		auto query = new SearchQuery(move(text), searchDirPath, MatchCase, sizeFrom, sizeTo,
			ExcludeHiddenAndSystem, ExcludeFolders, ExcludeFiles, cTimeFrom, cTimeTo);

		return query;
	}
}