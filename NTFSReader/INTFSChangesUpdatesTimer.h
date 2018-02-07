// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "FilesystemUpdatesPriority.h"
#include "typedefs.h"

namespace ntfs_reader {

// Used for testing, to replace standard sleep function.
class ISleeper { 
  public:

	// Suspends the calling thread on |time_ms|.
	virtual void Sleep(indexer_common::uint time_ms) const = 0;

	virtual indexer_common::uint GetTickCount() const = 0;

	virtual ~ISleeper() {};
};

class StandardWinApiSleeper : public ISleeper
{
  public:
	virtual void Sleep(indexer_common::uint time_ms) const override {
		Sleep(time_ms);
	}

	virtual indexer_common::uint GetTickCount() const override {
		return GetTickCount();
	}
};


// Interface for delaying filesystem events propagation to the SearchEngine, used by NTFSChangesWatcher.

class INTFSChangesUpdatesTimer
{
  public:
	virtual ~INTFSChangesUpdatesTimer() {};

	// Suspends the calling thread till the time, dependent on current FilesystemUpdatesPriority, is elapsed.
	void virtual SleepTillNextUpdate() = 0;

	void virtual UpdateNTFSChangesPriority(indexer_common::FilesystemUpdatesPriority new_priority) = 0;

	// If no |sleeper| will be set, a StandardWinApiSleeper class will be used.
	void SetSleeper(std::unique_ptr<ISleeper> sleeper) {
		sleeper_ = std::move(sleeper);
	};

  protected:
	std::unique_ptr<ISleeper> sleeper_;
};

} // namespace ntfs_reader