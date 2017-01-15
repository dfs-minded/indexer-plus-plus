// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "SearchQuery.h"
#include "typedefs.h"
// clang-format off

namespace CLIInterop
{
	public ref struct SearchQueryWrapper
	{
	   public:
		System::String^ Text = "";
		System::String^ SearchDirPath = "";
		bool MatchCase;
		bool UseRegex;

		int SizeFrom;
		int SizeTo;

		bool ExcludeHiddenAndSystem;
		bool ExcludeFolders;
		bool ExcludeFiles;

		System::DateTime^ CreatedTimeFrom;
		System::DateTime^ CreatedTimeTo;

		indexer_common::uSearchQuery ToUnmanagedQuery();
	};
}