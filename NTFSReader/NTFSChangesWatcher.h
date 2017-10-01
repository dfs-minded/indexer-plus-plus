// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <map>
#include "WindowsWrapper.h"

#include "CompilerSymb.h"
#include "Macros.h"
#include "typedefs.h"
#include "FilesystemChangesWatchingPriority.h"

namespace ntfs_reader {

    class NotifyNTFSChangedEventArgs;
    class USNJournalRecordsProvider;
    class USNJournalRecordsSerializer;
    class NTFSChangeObserver;


	// This class watches a NTFS volume changes. It uses a USN (Update Sequence Number) journal to get information about
	// files creation, modification or deletion.

    class NTFSChangesWatcher {
       public:
        NTFSChangesWatcher(char drive_letter, NTFSChangeObserver* observer);

        NO_COPY(NTFSChangesWatcher)

        ~NTFSChangesWatcher() = default;


        // Indicates weather the |NTFSChangesWatcher| must stop watching filesystem changes.

        std::atomic<bool> StopWatching;


        // Method which runs an infinite loop and waits for new update sequence number in a journal and calls ReadChanges()
        // to process NTFS changes. If |StopWatching| set to true, it deallocates the |buffer_| and returns.

        void WatchChanges(indexer_common::FilesystemChangesWatchingPriority = indexer_common::FilesystemChangesWatchingPriority::REALTIME);


		void UpdateNTFSChangesWatchingPriority(indexer_common::FilesystemChangesWatchingPriority new_priotity);


#ifdef SINGLE_THREAD
        // This function used for testing purposes. It will be called from the UI thread, in the single thread mode.
        void CheckUpdates();
#endif

       private:
        // Reads and parses a bunch of USN records from |buffer|. Then Forms a NTFS changed event args and calls
        // |ntfs_chabge_observer_|
        // to inform about changes.

        USN ReadChangesAndNotify(USN low_usn, char* buffer);


        // This function queries USN journal using winAPI and does not return until the new USN record created (which means
        // that new changes arrived).
        // In testing mode, reads calls test framework records provider class.

        bool WaitForNextUsn(PREAD_USN_JOURNAL_DATA read_journal_data) const;


        // Composes data structure for querying winAPI for waiting for next USN.

        std::unique_ptr<READ_USN_JOURNAL_DATA> GetWaitForNextUsnQuery(USN start_usn);


        // Composes data structure for reading the USN journal records into the |buffer_| and returns.

        std::unique_ptr<READ_USN_JOURNAL_DATA> GetReadJournalQuery(USN low_usn);


        // Calls winAPI function to read new records and fill with them |buffer|.
        // In testing mode, reads previously serialized records from file.

        bool ReadJournalRecords(PREAD_USN_JOURNAL_DATA journal_query, LPVOID buffer, DWORD& byte_count) const;


		static void DeleteFromCreatedAndChangedIfPresent(NotifyNTFSChangedEventArgs* args, indexer_common::uint ID);


		std::mutex mtx_;

		indexer_common::FilesystemChangesWatchingPriority ntfs_changes_watching_priority_;


        // The reference to the class, that consumes NTFS change event. When volume changes, NTFSChangesWatcher calls its
        // OnNTFSChanged method with change arguments.

        NTFSChangeObserver* ntfs_change_observer_;

        char drive_letter_;

        HANDLE volume_;

        std::unique_ptr<USN_JOURNAL_DATA> journal_;

        DWORDLONG journal_id_;

        USN last_usn_;

        static const int FILE_CHANGE_BITMASK;

        static const int kBufferSize;

        // Part of the test framework. Depending on commandline arguments, NTFSChangesWatcher can use previously recorded
        // USN journal records from file.

        bool read_volume_changes_from_file_;

        USNJournalRecordsSerializer* usn_records_serializer_;

        USNJournalRecordsProvider* usn_records_provider_;

		indexer_common::uint last_read_{ 0 };
		const indexer_common::uint kMinTimeBetweenReadMs{ 1000 };

		const std::map<indexer_common::FilesystemChangesWatchingPriority, indexer_common::uint> prioryti_to_min_time_between_read_ =
			std::map<indexer_common::FilesystemChangesWatchingPriority, indexer_common::uint> {
				{ indexer_common::FilesystemChangesWatchingPriority::REALTIME, kMinTimeBetweenReadMs },
				{ indexer_common::FilesystemChangesWatchingPriority::NORMAL, 10 * 1000 },
				{ indexer_common::FilesystemChangesWatchingPriority::BACKGROUND, 60 * 1000 }
		};
    };

} // namespace ntfs_reader