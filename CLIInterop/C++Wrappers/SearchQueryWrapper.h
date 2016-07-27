// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "typedefs.h"
#include "SearchQuery.h"
// clang-format off

namespace CLIInterop
{

	public ref struct SearchQueryWrapper
	{
	public:
		SearchQueryWrapper();

		System::String^ Text;
		System::String^ SearchDirPath;
		bool MatchCase;

                int SizeFrom;
                int SizeTo;

                bool ExcludeHiddenAndSystem;
                bool ExcludeFolders;
		bool ExcludeFiles;

		System::DateTime^ CreatedTimeFrom;
		System::DateTime^ CreatedTimeTo;

		SearchQuery* ToUnmanagedQuery();
	};

}