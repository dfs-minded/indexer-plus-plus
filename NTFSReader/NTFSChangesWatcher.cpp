// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "NTFSChangesWatcher.h"

#include <map>

#include "CommandlineArguments.h"
#include "CompilerSymb.h"
#include "FileInfo.h"
#include "HelperCommon.h"
#include "Log.h"
#include "WindowsWrapper.h"
#include "typedefs.h"

#include "NTFSChangeObserver.h"
#include "NotifyNTFSChangedEventArgs.h"
#include "USNJournalRecordsProvider.h"
#include "USNJournalRecordsSerializer.h"
#include "WinApiCommon.h"

namespace ntfs_reader {

    using namespace std;

    const int NTFSChangesWatcher::kBufferSize = 1024 * 1024 / 2;

    const int NTFSChangesWatcher::FILE_CHANGE_BITMASK =
        USN_REASON_RENAME_NEW_NAME | USN_REASON_SECURITY_CHANGE | USN_REASON_BASIC_INFO_CHANGE | USN_REASON_DATA_OVERWRITE |
        USN_REASON_DATA_TRUNCATION | USN_REASON_DATA_EXTEND | USN_REASON_CLOSE;

    NTFSChangesWatcher::NTFSChangesWatcher(char drive_letter, NTFSChangeObserver* observer)
        : StopWatching(false),
          ntfs_change_observer_(observer),
          drive_letter_(drive_letter),
          read_volume_changes_from_file_(false),
          usn_records_serializer_(nullptr),
          usn_records_provider_(nullptr) {
        usn_records_serializer_ = &USNJournalRecordsSerializer::Instance();

        auto records_filename = CommandlineArguments::Instance().ReplayUSNRecPath;
        read_volume_changes_from_file_ = !records_filename.empty();

        if (read_volume_changes_from_file_) {
            usn_records_serializer_->DeserializeAllRecords(records_filename);

            usn_records_provider_ = usn_records_serializer_->GetRecordsProvider(drive_letter_);
        } else {
            volume_ = WinApiCommon::OpenVolume(drive_letter_);

            journal_ = make_unique<USN_JOURNAL_DATA>();

            WinApiCommon::LoadJournal(volume_, journal_.get());

            journal_id_ = journal_->UsnJournalID;
            last_usn_ = journal_->NextUsn;
        }
    }

    void NTFSChangesWatcher::WatchChanges() {

        auto u_buffer = make_unique<char[]>(kBufferSize);

        auto read_journal_query = GetWaitForNextUsnQuery(last_usn_);

        while (true) {

            WaitForNextUsn(read_journal_query.get());

            uint current_time = GetTickCount();

            if (current_time < last_read_ + kMinTimeBetweenRead) {
                Sleep(last_read_ + kMinTimeBetweenRead - current_time);
            }

            last_read_ = GetTickCount();

            if (StopWatching) {
                // This thread is already detached, StopWatching indicates that it needs to finish execution and to free all
                // holding resources.
                delete this;
                return;
            }

            last_usn_ = ReadChanges(read_journal_query->StartUsn, u_buffer.get());
            read_journal_query->StartUsn = last_usn_;
        }
    }

    USN NTFSChangesWatcher::ReadChanges(USN low_usn, char* buffer) {

        DWORD byte_count;

        auto journal_query = GetReadJournalQuery(low_usn);

        if (!ReadJournalRecords(journal_query.get(), buffer, byte_count)) {
            WriteToOutput(METHOD_METADATA + HelperCommon::GetLastErrorString());
            return low_usn;
        }

        auto record = (USN_RECORD*)((USN*)buffer + 1);
        auto record_end = (USN_RECORD*)((BYTE*)buffer + byte_count);

        auto u_args = make_unique<NotifyNTFSChangedEventArgs>();

        for (; record < record_end; record = (USN_RECORD*)((BYTE*)record + record->RecordLength)) {
            // Part of the test framework.
            if (CommandlineArguments::Instance().SaveUSNJournalRecords) {
                usn_records_serializer_->SerializeRecord(*record, drive_letter_);
            }

            auto reason = record->Reason;
            auto ID = static_cast<uint>(record->FileReferenceNumber & 0x00000000FFFFFFFF);

            // WriteToOutput(wstring(L" ID = ") + to_wstring(ID) + L" " + Helper::GetReasonString(reason));

            // It is really strange, but some system files creating and deleting at the same time.
            if ((reason & USN_REASON_FILE_CREATE) && (reason & USN_REASON_FILE_DELETE)) {
                DeleteFromCreatedAndChangedIfPresent(u_args.get(), ID);
                continue;
            }

            if ((reason & USN_REASON_FILE_CREATE) && (reason & USN_REASON_CLOSE)) {

                auto created = u_args->CreatedItems.find(ID);

                if (created != u_args->CreatedItems.end()) {
                    delete created->second;
                    u_args->CreatedItems.erase(created);
                }

                auto new_fi = new FileInfo(*record, drive_letter_);
                u_args->CreatedItems[new_fi->ID] = new_fi;

            } else if ((reason & USN_REASON_FILE_DELETE) && (reason & USN_REASON_CLOSE)) {
                DeleteFromCreatedAndChangedIfPresent(u_args.get(), ID);

                u_args->DeletedItems.insert(ID);

            } else if (reason & FILE_CHANGE_BITMASK) {  // Something changed.

                FileInfo* file_to_update = nullptr;

                // Check if the file is present in Created or Changed collections in args. If yes - use this FileInfo for
                // new changes.

                auto created_it = u_args->CreatedItems.find(ID);
                if (created_it != u_args->CreatedItems.end()) {
                    file_to_update = created_it->second;
                }

                if (!file_to_update) {
                    auto changed_it = u_args->ChangedItems.find(ID);
                    if (changed_it != u_args->ChangedItems.end()) {
                        file_to_update = changed_it->second;
                    }
                }

                // If the file is not already in Created or Changed collections in args, it can be in UI and to make changes
                // safely we need to create new FileInfo, which will store all modifications.

                if (!file_to_update) {
                    file_to_update = new FileInfo(drive_letter_);
                    u_args->ChangedItems[ID] = file_to_update;
                }

                file_to_update->UpdateFromRecord(*record);

                // WriteToOutput(L"Changed fi = " + SerializeFileInfoHumanReadable(*file_to_update));
            }
        }

        if (u_args->CreatedItems.size() || u_args->DeletedItems.size() || u_args->ChangedItems.size()) {
            ntfs_change_observer_->OnNTFSChanged(move(u_args));
        }

        return *(USN*)buffer;
    }

    unique_ptr<READ_USN_JOURNAL_DATA> NTFSChangesWatcher::GetReadJournalQuery(USN low_usn) {

        auto query = make_unique<READ_USN_JOURNAL_DATA>();

        query->StartUsn = low_usn;
        query->ReasonMask = 0xFFFFFFFF;  // All bits.
        query->ReturnOnlyOnClose = FALSE;
        query->Timeout = 0;  			 // No timeout.
        query->BytesToWaitFor = 0;
        query->UsnJournalID = journal_id_;
        query->MinMajorVersion = 2;
        query->MaxMajorVersion = 2;

        return query;
    }

    bool NTFSChangesWatcher::ReadJournalRecords(PREAD_USN_JOURNAL_DATA journal_query, LPVOID buffer,
                                                DWORD& byte_count) const {
        if (read_volume_changes_from_file_) {
            return usn_records_provider_->GetNextJournalRecords(buffer, kBufferSize, &byte_count);
        }

#ifdef WIN32
        return DeviceIoControl(volume_, FSCTL_READ_USN_JOURNAL, journal_query, sizeof(*journal_query), buffer, kBufferSize,
                               &byte_count, nullptr) != 0;
#else
        return true;  // TODO Linux?
#endif
    }

    unique_ptr<READ_USN_JOURNAL_DATA> NTFSChangesWatcher::GetWaitForNextUsnQuery(USN start_usn) {

        auto query = make_unique<READ_USN_JOURNAL_DATA>();

        query->StartUsn = start_usn;
        query->ReasonMask = 0xFFFFFFFF;     // All bits.
        query->ReturnOnlyOnClose = FALSE;   // All entries.
        query->Timeout = 0;                 // No timeout.
        query->BytesToWaitFor = 1;          // Wait for this.
        query->UsnJournalID = journal_id_;  // The journal.
        query->MinMajorVersion = 2;
        query->MaxMajorVersion = 2;

        return query;
    }

    bool NTFSChangesWatcher::WaitForNextUsn(PREAD_USN_JOURNAL_DATA read_journal_data) const {

        DWORD bytes_read;
        bool ok = true;

        if (read_volume_changes_from_file_) {
            usn_records_provider_->ImitateWatchDelay();
        } else {
#ifdef WIN32  // TODO Linux
            // This function does not return until new USN record created.
            ok = DeviceIoControl(volume_, FSCTL_READ_USN_JOURNAL, read_journal_data, sizeof(*read_journal_data),
                                 &read_journal_data->StartUsn, sizeof(read_journal_data->StartUsn), &bytes_read,
                                 nullptr) != 0;
#endif
        }

        if (!ok) WriteToOutput(METHOD_METADATA + HelperCommon::GetLastErrorString());

        return ok;
    }

#ifdef SINGLE_THREAD
    void NTFSChangesWatcher::CheckUpdates() {
        USN nextUSN = last_usn_;
        auto u_buffer = make_unique<char[]>(kBufferSize);

        do {
            last_usn_ = nextUSN;
            nextUSN = ReadChanges(last_usn_, u_buffer.get());
        } while (last_usn_ != nextUSN);
    }
#endif

    void NTFSChangesWatcher::DeleteFromCreatedAndChangedIfPresent(NotifyNTFSChangedEventArgs* args, uint ID) {

        auto created_it = args->CreatedItems.find(ID);
        if (created_it != args->CreatedItems.end())  // If the file was created in this records group.
        {
            delete created_it->second;
            args->CreatedItems.erase(ID);
        }

        auto changed_it = args->ChangedItems.find(ID);
        if (changed_it != args->ChangedItems.end())  // If the file was modified in this records group.
        {
            delete changed_it->second;
            args->ChangedItems.erase(ID);
        }
    }

} // namespace ntfs_reader