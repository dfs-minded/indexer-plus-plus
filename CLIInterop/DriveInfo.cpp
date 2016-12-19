// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "DriveInfo.h"
// clang-format off

namespace CLIInterop 
{
	DriveInfo::DriveInfo(System::String^ VolumeLabel, char VolumeName) :
		Label(VolumeLabel), Name(VolumeName)
	{
	}
}