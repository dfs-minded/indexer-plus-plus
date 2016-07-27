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

class IndexChangeObserver;
class NTFSChangesWatcher;
class NotifyNTFSChangedEventArgs;
class MFTReader;
class FileInfo;
class Log;

// <mutex> and <thread> are not supported when compiling with /clr or /clr:pure, so this workaround needed.
namespace std {

#ifndef SINGLE_THREAD
class mutex;
class thread;
#endif
}

class IndexManager : public NTFSChangeObserver {
   public:
    IndexManager(char drive_letter, IndexChangeObserver* index_change_observer);

    NO_COPY(IndexManager)

    ~IndexManager();

    // Starts IndexManager in separate thread.
    void RunAsync();

#ifdef SINGLE_THREAD
    void CheckUpdates();
#endif

    char DriveLetter() const {
        return index_->DriveLetter();
    };

    std::wstring DriveLetterW() const {
        return drive_letter_w_;
    };

    const Index* GetIndex() const {
        return index_.get();
    }

    void EnableIndex();

    void DisableIndex();

    uint IndexRootID() const {
        return index_->RootID();
    }

    // Considering fact that bool is not atomic.

    /*bool*/ volatile int ReadingMFTFinished;

    /*bool*/ volatile int DisableIndexRequested;

   private:
    // Reads MFT, builds |index_| and watches NTFS changes.

    void Run();


    // Handles the NTFS changed event, applies all changes to |index_| and notifies |index_change_observer_|
    // about changes made in |index_|.

    virtual void OnNTFSChanged(std::unique_ptr<NotifyNTFSChangedEventArgs> u_args) override;


    // This method is only for testing and debug purposes!

    void CheckReaderResult(const MFTReadResult* raw_result) const;

    static void DisposeReaderResult(std::unique_ptr<MFTReadResult> u_read_res);


    // The index, which is uniquely hold and managed by this class instance.

    uIndex index_;


    // The reference for the class, which want to be notified, when the volume index has been changed.

    IndexChangeObserver* index_change_observer_;


    // An instance of the class, which watches NTFS changes. IndexManager runs its WatchChanges method
    // or uses CheckUpdates method in single thread mode.

    std::unique_ptr<NTFSChangesWatcher> ntfs_changes_watcher_;

    std::wstring drive_letter_w_;

    Log* logger_;

    uint64 start_time_;

#ifndef SINGLE_THREAD
    std::mutex* locker_;
#endif
};

typedef std::unique_ptr<IndexManager> uIndexManager;
