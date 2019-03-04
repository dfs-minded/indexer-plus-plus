// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#using <PresentationFramework.dll>
// clang-format off

namespace CLIInterop
{
	// Parses arguments in format similar to:
	// rawMFTPath="e:\+++\RawMFT.txt"  replayUSNRecPath="e:\+++\USNRecordsDB.txt"
	// For the correct keywords see their declaration in CommandlineArguments class.

	public ref class CmdArgumentsParser
	{
	  public:
		static void Init(System::Windows::StartupEventArgs^ e);

		// On the right click in windows explorer context menu on the folder, run Indexer++ with this directory filter.
        static System::String^ FilterDirPath = System::String::Empty;

	  private:
		literal System::String^ FilterDirPathArgName = "filterDirPath";
	};
}