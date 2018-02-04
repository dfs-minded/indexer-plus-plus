// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "NTFSChangesUpdatesTimer.h"

#include <algorithm>

#include "WindowsWrapper.h"
#include "Macros.h"

namespace ntfs_reader {

NTFSChangesUpdatesTimer::NTFSChangesUpdatesTimer(indexer_common::FilesystemUpdatesPriority priotity) 
	: ntfs_changes_watching_priority_(priotity) {
}

void ntfs_reader::NTFSChangesUpdatesTimer::SleepTillNextUpdate()
{
	indexer_common::uint current_time = GetTickCount();
	indexer_common::uint time_to_wait = 0;

	{
		std::unique_lock<std::mutex> locker(mtx_);
		time_to_wait = kPriorytiToMinTimeBetweenReadMs.at(ntfs_changes_watching_priority_);
	}

	while (current_time < last_read_ + time_to_wait) {
		auto sleep_time = std::min(kMinTimeBetweenReadMs, last_read_ + time_to_wait - current_time);
		Sleep(sleep_time);

		{
			std::unique_lock<std::mutex> locker(mtx_);
			// Meanwhile, priority might have changed.
			time_to_wait = kPriorytiToMinTimeBetweenReadMs.at(ntfs_changes_watching_priority_);
		}

		current_time = GetTickCount();
	}

	last_read_ = GetTickCount();
}

void ntfs_reader::NTFSChangesUpdatesTimer::UpdateNTFSChangesPriority(indexer_common::FilesystemUpdatesPriority new_priority) {
	std::unique_lock<std::mutex> locker(mtx_);
	ntfs_changes_watching_priority_ = new_priority;
}

} // namespace ntfs_reader 