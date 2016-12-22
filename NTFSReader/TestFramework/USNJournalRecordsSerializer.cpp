// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "USNJournalRecordsSerializer.h"

#include <fcntl.h>
#include <stdio.h>

#include "HelperCommon.h"
#include "Macros.h"
#include "WindowsWrapper.h"

#include "CommandlineArguments.h"
#include "Helper.h"
#include "IndexerDateTime.h"

namespace ntfs_reader {

    using namespace std;

    USNJournalRecordsSerializer& USNJournalRecordsSerializer::Instance() {
        static USNJournalRecordsSerializer instance_;
        return instance_;
    }

    USNJournalRecordsSerializer::USNJournalRecordsSerializer() : records_db_(nullptr) {
        if (!CommandlineArguments::Instance().SaveUSNJournalRecords) return;

        auto filename = "USNRecordsDB_" + to_string(IndexerDateTime::TicksNow()) + ".txt";

        records_db_ = fopen(filename.c_str(), "w");
#ifdef WIN32
        _setmode(_fileno(records_db_), _O_U8TEXT);
#endif

        NEW_MUTEX

#ifndef SINGLE_THREAD_LOG
        worker_ = new thread(&USNJournalRecordsSerializer::WriteToFileAsync, this);
#endif
    }

    USNJournalRecordsSerializer::~USNJournalRecordsSerializer() {
        if (records_db_) {
            fclose(records_db_);
        }
    }


    void USNJournalRecordsSerializer::SerializeRecord(const USN_RECORD& record, char drive_letter) {
        auto res = Helper::SerializeRecord(record, drive_letter);
        PLOCK_GUARD

#ifdef SINGLE_THREAD_LOG
        WriteToFile(move(res));
#else
        records_.push_back(move(res));
#endif  // SINGLE_THREAD_LOG
    }

    void USNJournalRecordsSerializer::DeserializeAllRecords(const wstring& records_filename) {
        vector<unique_ptr<USNJournalRecordsProvider>> providers;

        for (auto i = 0; i < 26; ++i) {
            providers.push_back(make_unique<USNJournalRecordsProvider>('A' + i));
        }
#ifdef WIN32
        FILE* records_in = _wfopen(records_filename.c_str(), L"r");
        _setmode(_fileno(records_in), _O_U8TEXT);
#else
        FILE* records_in = fopen(Helper::WStringToString(records_filename).c_str(), "r");
#endif

        wchar_t buffer[1001];

        while (fgetws(buffer, 1000, records_in)) {
            auto record_drive_pair = Helper::DeserializeRecord(buffer);
            providers[record_drive_pair.second - 'A']->AddRecord(move(record_drive_pair.first));
        }

        fclose(records_in);

        for (auto i = 0; i < 26; ++i) {
            if (!providers[i]->Empty()) {
                records_providers_[providers[i]->DriveLetter()] = move(providers[i]);
            }
        }
    }

    USNJournalRecordsProvider* USNJournalRecordsSerializer::GetRecordsProvider(char drive_letter) {
        if (records_providers_.find(drive_letter) == records_providers_.end()) return nullptr;

        return records_providers_[drive_letter].get();
    }


#ifdef SINGLE_THREAD_LOG

    void USNJournalRecordsSerializer::WriteToFile(wstring* msg) {
        fwprintf(records_db_, L"%s\n", (*msg).c_str());
        fflush(records_db_);
    }

#else

    void USNJournalRecordsSerializer::WriteToFileAsync() {
        while (true) {
            this_thread::sleep_for(chrono::seconds(2));

            PLOCK
            swap(records_, tmp_records_storage_);
            PUNLOCK

            for (const auto& msg : tmp_records_storage_) {
                fwprintf(records_db_, L"%s\n", msg.c_str());
            }

            tmp_records_storage_.clear();
            fflush(records_db_);
        }
    }

#endif  // SINGLE_THREAD_LOG

} // namespace ntfs_reader