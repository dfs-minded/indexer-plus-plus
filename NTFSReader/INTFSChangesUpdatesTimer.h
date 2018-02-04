// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "FilesystemUpdatesPriority.h"

namespace ntfs_reader {

// Interface for delaying filesystem events propagation to the SearchEngine, used by NTFSChangesWatcher.

class INTFSChangesUpdatesTimer
{
  public:
	virtual ~INTFSChangesUpdatesTimer() {};

	// Suspends the calling thread till the time, dependent on current FilesystemUpdatesPriority, is elapsed.
	void virtual SleepTillNextUpdate() = 0;

	void virtual UpdateNTFSChangesPriority(indexer_common::FilesystemUpdatesPriority new_priority) = 0;
};

} // namespace ntfs_reader