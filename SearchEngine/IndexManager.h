// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "CompilerSymb.h"
#include "MFTReadResult.h"
#include "Macros.h"
#include "NTFSChangeObserver.h"

#include "Index.h"

namespace indexer_common {
	class Log;
	struct BoolAtomicWrapper;
}

namespace ntfs_reader {
	class NTFSChangesWatcher;
	class NotifyNTFSChangedEventArgs;
}

namespace indexer {

    class IndexChangeObserver;

	// Manages access to the index of a volume, maintains index structure on filesystem changes.

    class IndexManager : public ntfs_reader::NTFSChangeObserver {
       public:
        IndexManager(char drive_letter, IndexChangeObserver* index_change_observer);

        NO_COPY(IndexManager)

        // Starts IndexManager in separate thread.
        void RunAsync();

#ifdef SINGLE_THREAD
        void CheckUpdates();
#endif

        char DriveLetter() const {
            return index_->DriveLetter();
        };

        std::wstring DriveLetterW() const {
            return index_->DriveLetterW();
        };

        Index* GetIndex() const {
            return index_.get();
        }

        void EnableIndex();

        void DisableIndex();

        indexer_common::uint IndexRootID() const {
            return index_->RootID();
        }

        bool ReadingMFTFinished() const;

        bool DisableIndexRequested() const;

       private:
        // Reads MFT, builds |index_| and watches NTFS changes.

        void Run();


        // Handles the NTFS changed event, applies all changes to |index_| and notifies |index_change_observer_|
        // about changes made in |index_|.

		virtual void OnNTFSChanged(std::unique_ptr<ntfs_reader::NotifyNTFSChangedEventArgs> u_args) override;


        // This method is for testing and debugging purposes.

		void CheckReaderResult(const indexer_common::MFTReadResult* raw_result) const;


		static void DisposeReaderResult(std::unique_ptr<indexer_common::MFTReadResult> u_read_res);


        // The index, which is uniquely hold and managed by this class instance.

        uIndex index_;


		std::unique_ptr<indexer_common::BoolAtomicWrapper> reading_mft_finished_;


        // User can unselect the drive.

		std::unique_ptr<indexer_common::BoolAtomicWrapper> disable_index_requested_;


        // The reference for the class, which want to be notified, when the volume index has been changed.

        IndexChangeObserver* index_change_observer_;


        // An instance of the class, which watches NTFS changes. IndexManager runs its WatchChanges method
        // or uses CheckUpdates method in single thread mode.

		std::unique_ptr<ntfs_reader::NTFSChangesWatcher> ntfs_changes_watcher_;

		indexer_common::Log* logger_;

		indexer_common::uint64 start_time_;
    };

    using uIndexManager = std::unique_ptr<IndexManager>;

} // namespace indexer