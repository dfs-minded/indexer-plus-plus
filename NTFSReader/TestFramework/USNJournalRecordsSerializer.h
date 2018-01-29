// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "WindowsWrapper.h"
#include "CompilerSymb.h"
#include "Macros.h"

#include "USNJournalRecordsProvider.h"


// Part of the test framework. Serializes and deserializes USN records for mocking calls to file system and OS layer.

namespace ntfs_reader {

    class USNJournalRecordsSerializer {
       public:
        NO_COPY(USNJournalRecordsSerializer)

        static USNJournalRecordsSerializer& Instance();


        // Serializes one single record to file.

        void SerializeRecord(const USN_RECORD& record, char drive_letter);


        // Creates records providers and populates them with corresponding to drive deserialized records.

        void DeserializeAllRecords(const std::wstring& records_filename);


        USNJournalRecordsProvider* GetRecordsProvider(char drive_letter);


       private:
        USNJournalRecordsSerializer();

        ~USNJournalRecordsSerializer();

        void WriteToFileAsync();

		const std::string kFilenamePrefix{ "USNRecordsDB_" };

		FILE* records_db_;
		
		std::map<char, std::unique_ptr<USNJournalRecordsProvider> > records_providers_;

#ifdef SINGLE_THREAD_LOG
        void WriteToFile(std::wstring msg);
#else

        std::mutex* mtx_;

        std::thread* worker_;

        std::list<std::wstring> records_;

        std::list<std::wstring> tmp_records_storage_;

#endif  // SINGLE_THREAD_LOG
    };

} // namespace ntfs_reader