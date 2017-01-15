// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "WindowsWrapper.h"

#include "MFTReadResult.h"
#include "macros.h"
#include "typedefs.h"

#include "MFTReader.h"

namespace ntfs_reader {

	// Reads and parses volume MFT using winAPI functions into FileInfo objects.

    class WinApiMFTReader : public MFTReader {
       public:
        explicit WinApiMFTReader(char drive_letter);

        NO_COPY(WinApiMFTReader)

        ~WinApiMFTReader() = default;

		virtual std::unique_ptr<indexer_common::MFTReadResult> ReadAllRecords() override;

       private:
        // Creates and fills MFT_ENUM_DATA data structure, which is used to query WinApi to get USN records.

        std::unique_ptr<MFT_ENUM_DATA> GetMFTEnumData() const;


        // Reads a chunk of records into buffer using WinApi function.

        bool ReadUSNRecords(const PMFT_ENUM_DATA mft_enum_data, const LPVOID buffer, DWORD& byte_count) const;


        // Finds the root directory ID.

		static indexer_common::uint FindRootID(const std::vector<indexer_common::FileInfo*>& data);


        // For each FileInfo that does not have parent FileInfo in the map assigns root FileInfo as a parent.

		static void AssignRootAsParent(const indexer_common::MFTReadResult& read_res);

        char drive_letter_;

        HANDLE volume_;

        static const int kBufferSize;
    };

} // namespace ntfs_reader