// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchQueryWrapper.h"

#include <limits>
#include <memory>

#include "SearchQueryBuilder.h"

#include "InteropHelper.h"

// clang-format off

namespace CLIInterop {

	indexer_common::uSearchQuery SearchQueryWrapper::ToUnmanagedQuery()
	{
		std::u16string text = Helper::ToU16string(Text);
		std::u16string searchDirPath = Helper::ToU16string(SearchDirPath);

		indexer_common::SearchQueryBuilder builder;
		builder.SetSearchText(text)
			.SetSearchDirPath(searchDirPath);

		if (MatchCase) builder.SetMatchCase();
		if (UseRegex) builder.SetUseRegex();

		builder.SetSizeFrom(SizeFrom);
		if (SizeTo != 0) builder.SetSizeTo(SizeTo);

		if (ExcludeHiddenAndSystem) builder.SetExcludeHiddenAndSystem();
		if (ExcludeFolders) builder.SetExcludeFolders();
		if (ExcludeFiles) builder.SetExcludeFiles();

		// Adjusting to the end of the day.
		CreatedTimeTo = CreatedTimeTo->AddDays(1).Subtract(System::TimeSpan::FromMilliseconds(1));

		auto cTimeFrom = (CreatedTimeFrom->Year < 1970) 
			? 0 
			: Helper::DateTimeToUnixTimeSeconds(CreatedTimeFrom);

		auto cTimeTo = (CreatedTimeTo->Year < 1970)
			? std::numeric_limits<indexer_common::uint>::max()
			: Helper::DateTimeToUnixTimeSeconds(CreatedTimeTo);

		builder.SetCreationTimeFrom(cTimeFrom).SetCreationTimeTo(cTimeTo);

		return builder.Build();
	}
}