// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <map>
#include <mutex>

#include "typedefs.h"
#include "FilesystemUpdatesPriority.h"

#include "INTFSChangesUpdatesTimer.h"

namespace ntfs_reader {

class NTFSChangesUpdatesTimer : public INTFSChangesUpdatesTimer
{
  public:
	NTFSChangesUpdatesTimer(indexer_common::FilesystemUpdatesPriority priotity);
	~NTFSChangesUpdatesTimer() = default;

	virtual void SleepTillNextUpdate() override;

	virtual void UpdateNTFSChangesPriority(indexer_common::FilesystemUpdatesPriority new_priority) override;
  
  private:
	const indexer_common::uint kMinTimeBetweenReadMs{ 500 };

	const std::map<indexer_common::FilesystemUpdatesPriority, indexer_common::uint> kPriorytiToMinTimeBetweenReadMs{
		std::map<indexer_common::FilesystemUpdatesPriority, indexer_common::uint>{
			{ indexer_common::FilesystemUpdatesPriority::REALTIME, kMinTimeBetweenReadMs },
			{ indexer_common::FilesystemUpdatesPriority::NORMAL, 10 * 1000 },
			{ indexer_common::FilesystemUpdatesPriority::BACKGROUND, 60 * 1000 }
		}
	};

	std::mutex mtx_;

	indexer_common::FilesystemUpdatesPriority ntfs_changes_watching_priority_;

	indexer_common::uint last_read_{ 0 };
};

} // namespace ntfs_reader