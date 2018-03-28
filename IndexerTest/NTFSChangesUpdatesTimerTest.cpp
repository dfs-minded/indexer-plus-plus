// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

#include "WindowsWrapper.h"

#include "CommandlineArguments.h"
#include "NTFSChangesUpdatesTimer.h"
#include "FilesystemUpdatesPriority.h"

#include "MockThreadSleeper.h"

namespace indexer_test {

	using ::testing::NiceMock;
	using ::testing::Expectation;
	using ::testing::Return;

	const indexer_common::uint64 kRealtimeDelayMs = 500;
	const indexer_common::uint64 kBackgroundDelayMs = 60 * 1000;

	TEST(NTFSChangesUpdatesTimerTest, TestUpdateNTFSChangesWatchingPriorityWorks) {
		using indexer_common::CommandlineArguments;
		CommandlineArguments::Instance().RawMFTPath = L"SerializedRawMFT/Disk_Z_RawMFT_11_25_16.txt";
		CommandlineArguments::Instance().ReplayUSNRecPath
			= L"SerializedUSNRecordsFiles/MultiAction/Creating600FilesWith0.5secIntervalAndDeletingAllAtTheEnd.txt";

		// Recorded USN records have the following behavior: 600 files were created with an interval in circa 0.4 sec,
		// followed by a delay in 1 sec, followed by deletion with no programmatical delays.
		// This gives approximately next timing of the recording:
		// 600*0.5 = 300 sec of files creation
		// OS-limited and dependent on CPU and memory load during the recording deletion of all created files.
		

		ntfs_reader::NTFSChangesUpdatesTimer object(indexer_common::FilesystemUpdatesPriority::REALTIME);
		
		std::unique_ptr<ntfs_reader::ISleeper> u_thread_sleeper = std::make_unique<NiceMock<MockThreadSleeper>>();
		// Grab the pointer before moving objects ownership.
		auto* nice_thread_sleeper = dynamic_cast<NiceMock<MockThreadSleeper>*>(u_thread_sleeper.get());

		Expectation realtime_1 = EXPECT_CALL(*nice_thread_sleeper, Sleep(kRealtimeDelayMs))
			.Times(1);

		// Using a MockThreadSleeper class, we return GetTickCount to get exactly 1 second to sleep.
		Expectation background = EXPECT_CALL(*nice_thread_sleeper, Sleep(1))
			.Times(1)
			.After(realtime_1);

		Expectation realtime_2 = EXPECT_CALL(*nice_thread_sleeper, Sleep(kRealtimeDelayMs))
			.Times(1)
			.After(background);

		auto start_time = 1000; // Setting a time which is easy to track and to do math with.

		EXPECT_CALL(*nice_thread_sleeper, GetTickCount())
			.Times(11)
			// first call of SleepTillNextUpdate()
			.WillOnce(Return(start_time)) // current_time; will not enter while loop.
			.WillOnce(Return(start_time)) // last_read_
			 // second call of SleepTillNextUpdate()
			.WillOnce(Return(start_time - 1)) // current_time; will step into while loop
			.WillOnce(Return(start_time + kRealtimeDelayMs)) // current_time; will step out of while loop
			.WillOnce(Return(start_time + kRealtimeDelayMs)) // last_read_; After this call priority will be changed to BG.
			 // third call of SleepTillNextUpdate()
			.WillOnce(Return(start_time + kRealtimeDelayMs + kBackgroundDelayMs - 1)) // current_time; will step into while loop
			.WillOnce(Return(start_time + kRealtimeDelayMs + kBackgroundDelayMs)) // current_time; will step out of while loop
			.WillOnce(Return(start_time + kRealtimeDelayMs + kBackgroundDelayMs)) // last_read_; Priority will be changed back to RT.
			 // fourth call of SleepTillNextUpdate()
			.WillOnce(Return(start_time + kRealtimeDelayMs + kBackgroundDelayMs - 1)) // current_time; will step into while loop
			.WillRepeatedly(Return(start_time + kRealtimeDelayMs + kBackgroundDelayMs + kRealtimeDelayMs)); // current_time; will step out of while loop

		object.SetSleeper(std::move(u_thread_sleeper));

		object.SleepTillNextUpdate(); // last_read_ is initially 0, so will not sleep on the first call.

		object.SleepTillNextUpdate();

		object.UpdateNTFSChangesPriority(indexer_common::FilesystemUpdatesPriority::BACKGROUND);
		object.SleepTillNextUpdate();

		object.UpdateNTFSChangesPriority(indexer_common::FilesystemUpdatesPriority::REALTIME);
		object.SleepTillNextUpdate();
	}

} // namespace indexer_test