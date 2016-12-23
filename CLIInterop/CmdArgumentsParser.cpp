// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "CmdArgumentsParser.h"

#include <string>
#include <vector>

#include "CommandlineArguments.h"

#include "InteropHelper.h"
// clang-format off

namespace CLIInterop
{
	// Parses arguments in format similar to:
	// rawMFTPath="e:\+++\RawMFT.txt"  replayUSNRecPath="e:\+++\USNRecordsDB.txt"
	// For the correct keywords see their declaration in CommandlineArguments class.

	void CmdArgumentsParser::Init(System::Windows::StartupEventArgs ^ e) 
	{
		std::vector<std::wstring> args;

		for (int i = 0; i < e->Args->Length; ++i) {
			System::String ^ arg = e->Args[i]->ToString();

			if (arg->Contains(FilterDirPathArgName))
				FilterDirPath = arg->Split('=')[1];
			else
				args.push_back(Helper::ToWstring(arg));
		}

		indexer_common::CommandlineArguments::Instance().Parse(args);
	}

}