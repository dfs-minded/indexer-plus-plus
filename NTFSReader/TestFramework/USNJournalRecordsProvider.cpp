// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "USNJournalRecordsProvider.h"

#include <string.h>

#include "../Common/Helpers.h"

namespace ntfs_reader {

	using namespace indexer_common;

    USNJournalRecordsProvider::USNJournalRecordsProvider(char drive_letter)
        : drive_letter_(drive_letter), next_record_(0), last_group_record_(-1) {
    }

    USNJournalRecordsProvider::~USNJournalRecordsProvider() {
        records_.clear();
    }

    bool USNJournalRecordsProvider::GetNextMFTRecords(LPVOID buffer, DWORD buffer_size, LPDWORD bytes_returned) {
        CopyMessagesToBuffer(buffer, bytes_returned);

        if (*bytes_returned == 8) {
            FindLastGroupRecord();
        }

        return true;
    }

    DWORD g_watch_delay = 500;

    void USNJournalRecordsProvider::ImitateWatchDelay() {
        Sleep(g_watch_delay);
    }

    bool g_group_finished = false;

    bool USNJournalRecordsProvider::GetNextJournalRecords(LPVOID buffer, DWORD buffer_size, LPDWORD bytes_returned) {
        CopyMessagesToBuffer(buffer, bytes_returned);

        if (next_record_ > last_group_record_) {
            if (!g_group_finished) {
                g_group_finished = true;
            } else {
                g_group_finished = false;
                FindLastGroupRecord();
            }
        }

        return true;
    }

	void USNJournalRecordsProvider::AddRecord(std::unique_ptr<USN_RECORD, std::function<void(USN_RECORD*)>> record) {
		records_.push_back(std::move(record));
    }

    char USNJournalRecordsProvider::DriveLetter() const {
        return drive_letter_;
    }

    bool USNJournalRecordsProvider::Empty() const {
        return records_.empty();
    }

    uint64 g_ms_multiplier = 10000;
    uint64 g_min_group_time = 1000 * g_ms_multiplier;
    uint64 g_max_dist_event_ms = 10 * g_ms_multiplier;

    void USNJournalRecordsProvider::FindLastGroupRecord() {
        if (next_record_ >= static_cast<int>(records_.size())) {
            last_group_record_ = records_.size() - 1;
            return;
        }

        auto time = helpers::LargeIntegerToInt64(records_[next_record_]->TimeStamp);
        auto min_end_to = time + g_min_group_time;
        auto irecord = next_record_ + 1;

        while (irecord < static_cast<int>(records_.size())) {
            auto current_time = helpers::LargeIntegerToInt64(records_[irecord]->TimeStamp);
            auto diff = current_time - time;

            if (diff > g_max_dist_event_ms && current_time > min_end_to) {
                break;
            }

            time = current_time;
            ++irecord;
        }

        last_group_record_ = irecord - 1;
    }

    void USNJournalRecordsProvider::CopyMessagesToBuffer(LPVOID& buffer, LPDWORD& bytes_returned) {

        *(DWORDLONG*)buffer = 0;  // First start FRN is returned, not needed for RecordsProvider.
        auto p = (char*)buffer;
        p += sizeof(DWORDLONG);
        *bytes_returned = sizeof(DWORDLONG);
        auto to = last_group_record_ + 1;

        while (next_record_ < to) {
            auto u_record = move(records_[next_record_]);
            int record_length = u_record->RecordLength;
            memcpy(p, u_record.get(), record_length);
            p = p + record_length;
            *bytes_returned += record_length;
            ++next_record_;
        }
    }

} // namespace ntfs_reader