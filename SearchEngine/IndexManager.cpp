// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "IndexManager.h"

#include <string.h>
#include <iostream>
#include <mutex>
#include <thread>

#include "AsyncLog.h"
#include "CompilerSymb.h"
#include "FileInfo.h"
#include "HelperCommon.h"
#include "Macros.h"
#include "OneThreadLog.h"


#include "MFTReader.h"
#include "MFTReaderFactory.h"
#include "NTFSChangesWatcher.h"
#include "TestFramework/ReaderDataComparator.h"
#include "WinApiCommon.h"
#include "WinApiMftReader.h"

#include "FileInfoHelper.h"
#include "IndexChangeObserver.h"
#include "NotifyIndexChangedEventArgs.h"

using namespace std;

IndexManager::IndexManager(char drive_letter, IndexChangeObserver* index_change_observer)
    : ReadingMFTFinished(false),
      DisableIndexRequested(false),
      index_(make_unique<Index>(drive_letter)),
      index_change_observer_(index_change_observer),
      drive_letter_w_(1, drive_letter),
      start_time_(0) {

    NEW_LOCKER

    GET_LOGGER
}

IndexManager::~IndexManager() {
    DELETE_LOCKER
}

void IndexManager::RunAsync() {

    logger_->Info(METHOD_METADATA + L"Called for drive " + DriveLetterW());

#ifndef SINGLE_THREAD

    thread worker_thread(&IndexManager::Run, this);

    HelperCommon::SetThreadName(&worker_thread, (string("ReaderAndWatcher ") + DriveLetter()).c_str());

    worker_thread.detach();

#else
    Run();
#endif  // !SINGLE_THREAD
}

#ifdef SINGLE_THREAD

void IndexManager::CheckUpdates() {
    if (!ReadingMFTFinished) return;

    logger_->Debug(METHOD_METADATA + L"Called for drive " + DriveLetterW());

    ntfs_changes_watcher_->CheckUpdates();
}

#endif  // SINGLE_THREAD

void IndexManager::EnableIndex() {

    DisableIndexRequested = false;

    RunAsync();
}

void IndexManager::DisableIndex() {

    DisableIndexRequested = true;

    // If reading MFT not finished yet (and the index is not populated and filesystem watcher is not created etc.), we
    // have nothing to clear.
    if (!ReadingMFTFinished) {
        index_change_observer_->OnVolumeStatusChanged(DriveLetter());
        return;
    }

    // IndexManager will be no more responsible for |ntfs_changes_watcher_| deletion.
    // It must be deleted itself in its worker thread.
    auto* watcher         = ntfs_changes_watcher_.release();
    watcher->StopWatching = true;

    auto* data         = index_->ReleaseData();
    ReadingMFTFinished = false;

    vector<const FileInfo *> new_items, old_items, changed_items;
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

void IndexManager::Run() {

    if (ReadingMFTFinished || DisableIndexRequested) return;

    auto u_reader = MFTReaderFactory::CreateReader(DriveLetter());

    logger_->Debug(METHOD_METADATA + L"Started for drive " + DriveLetterW());
    TIK

    auto u_read_res = u_reader->ReadAllRecords();

    // It's here only for debug and testing.
    // CheckReaderResult(u_read_res.get());

    // Reading records is long-time operation, meanwhile the volume could be disabled by user.
    if (DisableIndexRequested) {
        DisposeReaderResult(move(u_read_res));
        return;
    }

    index_->SetData(move(u_read_res->Data));
    index_->RootID(u_read_res->Root->ID);

    u_read_res.reset();

    TOK(L"Reading MFT finished for drive " + DriveLetterW())

    index_->BuildTree();
    TOK(L"Build tree for drive " + DriveLetterW())

#ifdef WIN_API_MFT_READ
    // Need to read sizes and timestamps separately. In Raw MFT read this data retrieved directly from MFT.
    for (auto* fi : data) {
        if (!fi) continue;
        WinApiCommon::GetSizeAndTimestamps(TODO, fi);
    }
#endif

    ReadingMFTFinished = true;

    index_->CalculateDirsSizes();

    index_change_observer_->OnVolumeStatusChanged(DriveLetter());

    // Form new index change event args and populate them with index files.
    auto* data = index_->LockData();

    vector<const FileInfo *> new_items, old_items, changed_items;
    new_items.reserve(data->size());

    for (auto* fi : *data) {
        if (!fi) continue;
        new_items.push_back(fi);
    }

    index_->UnlockData();

    auto p_args = make_shared<NotifyIndexChangedEventArgs>(move(new_items), move(old_items), move(changed_items));
    logger_->Debug(METHOD_METADATA + p_args->ToWString());

    index_change_observer_->OnIndexChanged(p_args);

#if defined(WATCH_CHANGES) && !defined(SINGLE_THREAD)

    ntfs_changes_watcher_ = make_unique<NTFSChangesWatcher>(DriveLetter(), this);
    ntfs_changes_watcher_->WatchChanges();

#endif
}

void IndexManager::OnNTFSChanged(unique_ptr<NotifyNTFSChangedEventArgs> u_args) {

    logger_->Debug(METHOD_METADATA + u_args->ToWString());

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

        index_->RemoveNode(old_fi);

        logger_->Debug(METHOD_METADATA + L"Removed from the Index node ID = " + to_wstring(ID));

        index_->UpdateParentDirsSize(old_fi, -1 * old_fi->SizeReal);
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
                           reinterpret_cast<const wchar_t*>(new_fi->GetName());

            if (new_fi->NameLength != existing->NameLength ||
                memcmp(new_fi->GetName(), existing->GetName(), existing->NameLength) != 0) {
                log_msg += wstring(L" OldName = ") + reinterpret_cast<const wchar_t*>(existing->GetName());
            }

            logger_->Warning(log_msg);

            delete new_fi;
            continue;
        }

        index_->InsertNode(new_fi);

        auto u_full_name = FileInfoHelper::GetPath(*new_fi, true);
        WinApiCommon::GetSizeAndTimestamps(*u_full_name.get(), new_fi);  // TODO Use from USN record serializer

        index_->UpdateParentDirsSize(new_fi, new_fi->SizeReal);
        new_items.push_back(new_fi);
    }

    // Handle modified files.
    for (const auto& keyval : u_args->ChangedItems) {

        FileInfo* fi_with_changes = keyval.second;

        auto fi_to_update = index_->GetNode(fi_with_changes->ID);
        if (!fi_to_update) {
            logger_->Warning(METHOD_METADATA + L"Cannot process file change. No index entry with ID = " +
                             to_wstring(fi_with_changes->ID));
            continue;
        }

        int size_delta = -1 * fi_to_update->SizeReal;

        // Update parent, if changed.
        if (fi_to_update->ParentID != fi_with_changes->ParentID) {

            index_->RemoveNode(fi_to_update);
            index_->UpdateParentDirsSize(fi_to_update, size_delta);
            size_delta = 0;

            fi_to_update->ParentID = fi_with_changes->ParentID;

            index_->InsertNode(fi_to_update);
        }

        auto u_path = FileInfoHelper::GetPath(*fi_to_update, true);
        WinApiCommon::GetSizeAndTimestamps(*u_path.get(), fi_to_update);

        size_delta += fi_to_update->SizeReal;

        index_->UpdateParentDirsSize(fi_to_update, size_delta);

        // Update all other attributes.
        /*if (fi_with_changes->SizeAllocated)
        {
                fi_to_update->SizeAllocated = fi_with_changes->SizeAllocated;
        }*/

        fi_to_update->FileAttributes = fi_with_changes->FileAttributes;

        fi_to_update->CopyAndSetName(fi_with_changes->GetName(), fi_with_changes->NameLength);

        delete fi_with_changes;

        changed_items.push_back(fi_to_update);
    }

    if (!old_items.empty() || !new_items.empty() || !changed_items.empty()) {
        auto p_args = make_shared<NotifyIndexChangedEventArgs>(move(new_items), move(old_items), move(changed_items));
        logger_->Debug(METHOD_METADATA + p_args->ToWString());

        index_change_observer_->OnIndexChanged(p_args);
    }

    index_->UnlockData();
}

void IndexManager::CheckReaderResult(const MFTReadResult* raw_result) const {

    // Get data from WinAPI MFT reader.
    WinApiMFTReader win_api_reader(DriveLetter());
    auto win_api_reader_result = win_api_reader.ReadAllRecords();
    auto win_api_index         = make_unique<Index>(DriveLetter());
    win_api_index->SetData(move(win_api_reader_result->Data));
    win_api_index->RootID(win_api_reader_result->Root->ID);
    win_api_index->BuildTree();
    win_api_reader_result.reset();
    auto win_api_data = win_api_index->LockData();

    for (auto* fi : *win_api_data) {
        if (!fi) continue;

        auto u_full_name = FileInfoHelper::GetPath(*fi, true);
        WinApiCommon::GetSizeAndTimestamps(*u_full_name.get(), fi);
    }

    ReaderDataComparator comparator;
    comparator.Compare(*raw_result->Data, *raw_result->Root, L"RawReader", *win_api_data, *win_api_index->Root(),
                       L"WinAPIReader");

    win_api_index->UnlockData();
}

void IndexManager::DisposeReaderResult(unique_ptr<MFTReadResult> u_read_res) {

    u_read_res->Root = nullptr;

    for (FileInfo* fi : *u_read_res->Data)
        delete fi;

    u_read_res = nullptr;
}
