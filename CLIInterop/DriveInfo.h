// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once
// clang-format off

namespace CLIInterop 
{
	public ref class DriveInfo
	{
	   public:
		DriveInfo(System::String^ Label, char Name);

		System::String^ Label; 

		System::Char Name;
	};
}
