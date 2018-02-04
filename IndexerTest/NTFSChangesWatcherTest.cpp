// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#include <vector>

#include "CommandlineArguments.h"
#include "NTFSChangesWatcher.h"
#include "FilesystemUpdatesPriority.h"
#include "NotifyIndexChangedEventArgs.h"

#include "MockNTFSChangeObserver.h"

namespace indexer_test {

	const int kMaxDelayDeltaMs{ 300 };

	const auto kMaxRealtimeDelayMs{ indexer_common::kPriorytiToMinTimeBetweenReadMs.at(
		indexer_common::FilesystemUpdatesPriority::REALTIME) + kMaxDelayDeltaMs };

	const auto kMaxBackgroundDelayMs{ indexer_common::kPriorytiToMinTimeBetweenReadMs.at(
		indexer_common::FilesystemUpdatesPriority::BACKGROUND) + kMaxDelayDeltaMs };

	std::unique_ptr<ntfs_reader::NTFSChangesWatcher> object;
	bool background_mode_requested;
	bool changed_back_to_realtime_mode_requested;
	


	void TestChangingPriority(ntfs_reader::uNotifyNTFSChangedEventArgs args) {
		static indexer_common::ulong last_notification_time;
		static bool last_time_of_notification_initialized;
		static indexer_common::ulong start_time;

		auto notified_at = GetTickCount64();

		if (!last_time_of_notification_initialized) {
			start_time = last_notification_time = GetTickCount64();
			last_time_of_notification_initialized = true;
			return;
		}

		auto elapsed_since_started_observing = GetTickCount64() - start_time;
		auto interval_between_last_two_notifications = notified_at - last_notification_time;
		last_notification_time = notified_at;

		std::cerr << "elapsed_since_started_observing " << elapsed_since_started_observing << std::endl;
		std::cerr << "interval_between_last_two_notifications  " << interval_between_last_two_notifications << std::endl;
		std::cerr << "kMaxRealtimeDelayMs  " << kMaxRealtimeDelayMs << std::endl;


		if (elapsed_since_started_observing < 20 * 1000) { // less then 20 sec past from the first notification
			ASSERT_TRUE(interval_between_last_two_notifications < kMaxRealtimeDelayMs);
		}
		else if (elapsed_since_started_observing < 140 * 1000) { // between 20 sec and 140 sec
			if (!background_mode_requested) {
				object->UpdateNTFSChangesWatchingPriority(indexer_common::FilesystemUpdatesPriority::BACKGROUND);
				background_mode_requested = true;
				return;
			}
			ASSERT_TRUE(interval_between_last_two_notifications < kMaxBackgroundDelayMs);
		}
		else if (elapsed_since_started_observing < 160 * 1000) { // between 140 sec and 160 sec
			if (!changed_back_to_realtime_mode_requested) {
				object->UpdateNTFSChangesWatchingPriority(indexer_common::FilesystemUpdatesPriority::REALTIME);
				changed_back_to_realtime_mode_requested = true;
				return;
			}
			ASSERT_TRUE(interval_between_last_two_notifications < kMaxRealtimeDelayMs);
		}
	}

	TEST(NTFSChangesWatcherTest, TestUpdateNTFSChangesWatchingPriorityWorks) {
		using indexer_common::CommandlineArguments;
		CommandlineArguments::Instance().RawMFTPath = L"SerializedRawMFT/Disk_Z_RawMFT_11_25_16.txt";
		CommandlineArguments::Instance().ReplayUSNRecPath
			= L"SerializedUSNRecordsFiles/MultiAction/Creating600FilesWith0.5secIntervalAndDeletingAllAtTheEnd.txt";

		// Recorded USN records have the following behavior: 600 files were created with an interval in circa 0.4 sec,
		// followed by a delay in 1 sec, followed by deletion with no programmatical delays.
		// This gives approximately next timing of the recording:
		// 600*0.5 = 300 sec of files creation
		// OS-limited and dependent on CPU and memory load during the recording deletion of all created files.

		MockNTFSChangeObserver observer(&TestChangingPriority);
		object = std::make_unique<ntfs_reader::NTFSChangesWatcher>('Z', &observer);
		object->WatchChanges();
	}

} // namespace indexer_test