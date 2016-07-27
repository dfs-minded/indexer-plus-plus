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