// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "IndexManager.h"

#include <string.h>
#include <iostream>
#include <mutex>
#include <thread>

#include "AsyncLog.h"
#include "CLRCompilantWrappers.h"
#include "CompilerSymb.h"
#include "FileInfo.h"
#include "FileInfoHelper.h"
#include "../Common/Helpers.h"
#include "macros.h"
#include "OneThreadLog.h"

#include "MFTReaderFactory.h"
#include "NTFSChangesWatcher.h"
#include "TestFramework/ReaderDataComparator.h"
#include "WinApiCommon.h"
#include "WinApiMftReader.h"

#include "IndexChangeObserver.h"
#include "NotifyIndexChangedEventArgs.h"

namespace indexer {

	using std::string;
	using std::wstring;
	using std::to_wstring;
	using std::vector;
	using std::unique_ptr;
	using std::make_unique;
	using std::make_shared;
	using std::move;

    using namespace indexer_common;

    IndexManager::IndexManager(char drive_letter, IndexChangeObserver* index_change_observer)
        : reading_mft_finished_(make_unique<BoolAtomicWrapper>(false)),
          disable_index_requested_(make_unique<BoolAtomicWrapper>(false)),
          index_(make_unique<Index>(drive_letter)),
          index_change_observer_(index_change_observer),
          start_time_(0) {

        GET_LOGGER
    }

    void IndexManager::RunAsync() {

        logger_->Debug(METHOD_METADATA + L"Called for drive " + DriveLetterW());

#ifndef SINGLE_THREAD

        std::thread worker_thread(&IndexManager::Run, this);

        helpers::SetThreadName(&worker_thread, (string("ReaderAndWatcher ") + DriveLetter()).c_str());

        worker_thread.detach();

#else
        Run();
#endif  // !SINGLE_THREAD
    }

#ifdef SINGLE_THREAD

    void IndexManager::CheckUpdates() {

        if (!ReadingMFTFinished()) return;

        logger_->Debug(METHOD_METADATA + L"Called for drive " + DriveLetterW());

        if (!ntfs_changes_watcher_) ntfs_changes_watcher_ = 
			make_unique<ntfs_reader::NTFSChangesWatcher>(DriveLetter(), this);

        ntfs_changes_watcher_->CheckUpdates();
    }

#endif  // SINGLE_THREAD

    void IndexManager::EnableIndex() {

        disable_index_requested_->store(false);

        RunAsync();
    }

    void IndexManager::DisableIndex() {

        disable_index_requested_->store(true);

        // If reading MFT not finished yet (and the index is not populated and filesystem watcher is not created etc.), we
        // have nothing to clear.
        if (!ReadingMFTFinished()) {
            index_change_observer_->OnVolumeStatusChanged(DriveLetter());
            return;
        }

        // IndexManager will be no more responsible for |ntfs_changes_watcher_| deletion.
        // It must be deleted itself in its worker thread.
        if (ntfs_changes_watcher_) {
            auto* watcher = ntfs_changes_watcher_.release();
            watcher->StopWatching = true;
        }

        index_->LockData();

        auto* data = index_->ReleaseData();

        index_->UnlockData();

        reading_mft_finished_->store(false);

        vector<const FileInfo*> new_items, old_items, changed_items;
        old_items.reserve(data->size());
        for (auto* fi : *data) {
            if (!fi) continue;
            old_items.push_back(fi);
        }

        auto p_args = make_shared<NotifyIndexChangedEventArgs>(move(new_items), move(old_items), move(changed_items));
        logger_->Debug(METHOD_METADATA + p_args->ToWString());

        index_change_observer_->OnVolumeStatusChanged(DriveLetter());
        index_change_observer_->OnIndexChanged(p_args);
    }

    bool IndexManager::ReadingMFTFinished() const {
        return reading_mft_finished_->load();
    }

    bool IndexManager::DisableIndexRequested() const {
        return disable_index_requested_->load();
    }

    void IndexManager::Run() {

        if (ReadingMFTFinished() || DisableIndexRequested()) return;

        auto u_reader = ntfs_reader::MFTReaderFactory::CreateReader(DriveLetter());

        logger_->Debug(METHOD_METADATA + L"Started for drive " + DriveLetterW());
        TIK

            auto u_read_res = u_reader->ReadAllRecords();

        // It's here for debug and testing: compares results which were retrieved from win API and
        // from direct parsing of the filesystem.
        // CheckReaderResult(u_read_res.get());

        // Reading records is long-time operation, meanwhile the volume could be disabled by user.
        if (DisableIndexRequested()) {
            DisposeReaderResult(move(u_read_res));
            return;
        }

        index_->LockData();

        index_->SetData(move(u_read_res->Data));
        index_->RootID(u_read_res->Root->ID);

        u_read_res.reset();

        TOK(L"Reading MFT finished for drive " + DriveLetterW())

        index_->BuildTree();

        index_->UnlockData();

        TOK(L"Build tree finished for drive " + DriveLetterW())

#ifdef WIN_API_MFT_READ
        // Need to read sizes and timestamps separately. In Raw MFT read this data retrieved directly from the MFT.
        for (auto* fi : data) {
            if (!fi) continue;
            WinApiCommon::GetSizeAndTimestamps(TODO, fi);
        }
#endif

        reading_mft_finished_->store(true);

        index_change_observer_->OnVolumeStatusChanged(DriveLetter());

        auto* data = index_->LockData();

        index_->CalculateDirsSizes();

        index_->UnlockData();

        // Form new index change event args and populate them with index files (all added to the new_items collection).

        vector<const FileInfo *> new_items, old_items, changed_items;
        new_items.reserve(data->size());

        for (auto* fi : *data) {
            if (!fi) continue;
            new_items.push_back(fi);
        }

        auto p_args = make_shared<NotifyIndexChangedEventArgs>(move(new_items), move(old_items), move(changed_items));
        logger_->Debug(METHOD_METADATA + p_args->ToWString());

        index_change_observer_->OnIndexChanged(p_args);

#if defined(WATCH_CHANGES) && !defined(SINGLE_THREAD)

        ntfs_changes_watcher_ = make_unique<ntfs_reader::NTFSChangesWatcher>(DriveLetter(), this);
        ntfs_changes_watcher_->WatchChanges();

#endif
    }

	void IndexManager::OnNTFSChanged(unique_ptr<ntfs_reader::NotifyNTFSChangedEventArgs> u_args) {

        logger_->Debug(METHOD_METADATA + L" drive " + DriveLetterW() + u_args->ToWString());

        // Files for merging with search result.
        vector<const FileInfo*> new_items;

        // Files for deletion in search result.
        vector<const FileInfo*> old_items;

        // Files need to update in search result.
        vector<const FileInfo*> changed_items;

        index_->LockData();

        // Handle deleted files. First in arguments are children, than their parents. Deleted files must be deleted from
        // the Index before the new one will be added to avoid ID collisions.
        for (auto ID : u_args->DeletedItems) {

            auto old_fi = index_->GetNode(ID);
            // It is OK that we receive delete messages for files, which are not in the Index. They could be created,
            // modified and  deleted in one changes session, so in args we can receive only deleted IDs as accumulated
            // result. We still need to check if this file not left in the Index, because creation and deletion of the
            // file could be handled in different chunks of NTFS changed messages.
            if (!old_fi) {
                logger_->Warning(METHOD_METADATA + L"Cannot delete file. No index entry with ID = " + to_wstring(ID));
                continue;
            }

            index_->UpdateParentDirsSize(old_fi, -1 * old_fi->SizeReal);

            index_->RemoveNode(old_fi);

            logger_->Debug(METHOD_METADATA + L"Removed from the Index node ID = " + to_wstring(ID));


            old_items.push_back(old_fi);
        }

        // Handle created files.
        for (const auto& keyval : u_args->CreatedItems) {

            FileInfo* new_fi = keyval.second;

            // If file already exist - continue.
            auto existing = index_->GetNode(new_fi->ID);
            if (existing != nullptr) {

                auto log_msg = METHOD_METADATA + L"Cannot insert new. File with such ID already exists in index. ID = " +
                               to_wstring(new_fi->ID) + wstring(L" Name = ") +
                               helpers::Char16ToWstring(new_fi->GetName());

                if (new_fi->NameLength != existing->NameLength ||
                    memcmp(new_fi->GetName(), existing->GetName(), existing->NameLength) != 0) {
                    log_msg += wstring(L" OldName = ") + helpers::Char16ToWstring(existing->GetName());
                }

                logger_->Warning(log_msg);

                delete new_fi;
                continue;
            }

            if (index_->InsertNode(new_fi)) {
                auto u_full_name = FileInfoHelper::GetPath(*new_fi, true);
                ntfs_reader::WinApiCommon::GetSizeAndTimestamps(u_full_name.get(), new_fi);  // TODO Use from USN record serializer
                index_->UpdateParentDirsSize(new_fi, new_fi->SizeReal);

                new_items.push_back(new_fi);

            } else {
                delete new_fi;
            }
        }

        // Handle modified files.
        for (const auto& keyval : u_args->ChangedItems) {

            FileInfo* fi_with_changes = keyval.second;

            auto fi_to_update = index_->GetNode(fi_with_changes->ID);
            if (!fi_to_update) {
                logger_->Warning(METHOD_METADATA + L"Cannot process file change. No index entry with ID = " +
                                 to_wstring(fi_with_changes->ID));

                delete fi_with_changes;
                continue;
            }

            // Update parent, if changed (file moved).

            int size_delta = -1 * fi_to_update->SizeReal;

            // Assume that the file we want to update is in the Index. It could change if the file location has been
            // changed and the InsertNode operation was unsuccessful.
            bool fi_to_update_is_in_index = true;

            if (fi_to_update->ParentID != fi_with_changes->ParentID) {

                logger_->Debug(METHOD_METADATA + L"File was moved, new ParentID = " +
                               to_wstring(fi_with_changes->ParentID));

                index_->UpdateParentDirsSize(fi_to_update, size_delta);
                size_delta = 0;

                index_->RemoveNode(fi_to_update);

                fi_to_update->ParentID = fi_with_changes->ParentID;

                fi_to_update_is_in_index = index_->InsertNode(fi_to_update);
            }

            if (fi_to_update_is_in_index) {

                auto u_path = FileInfoHelper::GetPath(*fi_to_update, true);
				ntfs_reader::WinApiCommon::GetSizeAndTimestamps(u_path.get(), fi_to_update);

                size_delta += fi_to_update->SizeReal;

                index_->UpdateParentDirsSize(fi_to_update, size_delta);

                // Currently SizeAllocated is not supported
                // if (fi_with_changes->SizeAllocated)
                //	fi_to_update->SizeAllocated = fi_with_changes->SizeAllocated;

                fi_to_update->FileAttributes = fi_with_changes->FileAttributes;

                fi_to_update->CopyAndSetName(fi_with_changes->GetName(), fi_with_changes->NameLength);

                changed_items.push_back(fi_to_update);

                // If the file was moved, and there is no parent node for the new location in the Index - delete this file.
            } else {
                logger_->Warning(METHOD_METADATA +
                                 L"Cannot process file change. No parent index entry for new file location." +
                                 L" Changed file ID:" + to_wstring(fi_with_changes->ID) + L" Searched ParentID:" +
                                 to_wstring(fi_with_changes->ParentID));

                old_items.push_back(fi_to_update);
            }

            delete fi_with_changes;
        }

        index_->UnlockData();

        if (!old_items.empty() || !new_items.empty() || !changed_items.empty()) {
            auto p_args = make_shared<NotifyIndexChangedEventArgs>(move(new_items), move(old_items), move(changed_items));
            logger_->Debug(METHOD_METADATA + p_args->ToWString());

            index_change_observer_->OnIndexChanged(p_args);
        }
    }

    void IndexManager::CheckReaderResult(const MFTReadResult* raw_result) const {

        // Get data from WinAPI MFT reader.
		ntfs_reader::WinApiMFTReader win_api_reader(DriveLetter());
        auto win_api_reader_result = win_api_reader.ReadAllRecords();
        auto win_api_index = make_unique<Index>(DriveLetter());

        auto win_api_data = win_api_index->LockData();

        win_api_index->SetData(move(win_api_reader_result->Data));
        win_api_index->RootID(win_api_reader_result->Root->ID);
        win_api_index->BuildTree();
        win_api_reader_result.reset();

        for (auto* fi : *win_api_data) {
            if (!fi) continue;

            auto u_full_name = FileInfoHelper::GetPath(*fi, true);
			ntfs_reader::WinApiCommon::GetSizeAndTimestamps(u_full_name.get(), fi);
        }

        win_api_index->UnlockData();

		ntfs_reader::ReaderDataComparator comparator;
        comparator.Compare(*raw_result->Data, *raw_result->Root, L"RawReader", *win_api_data, *win_api_index->Root(),
                           L"WinAPIReader");
    }

    void IndexManager::DisposeReaderResult(unique_ptr<MFTReadResult> u_read_res) {

        u_read_res->Root = nullptr;

        for (FileInfo* fi : *u_read_res->Data)
            delete fi;

        u_read_res = nullptr;
    }

} // namespace indexer