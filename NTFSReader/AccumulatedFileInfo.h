// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "FileInfo.h"
#include "macros.h"

#include "NTFSDataStructures.h"

namespace ntfs_reader {

	// During the MFT parsing intermediate USN records parsing result stored in AccumulatedFileInfo objects.
    class AccumulatedFileInfo {
       public:
		AccumulatedFileInfo(char drive_letter, indexer_common::uint id);

        NO_COPY(AccumulatedFileInfo)

        ~AccumulatedFileInfo();

		indexer_common::uint ID() const {
            return fi_->ID;
        }

        FilenameFlagsEnum UsedFilenameFlags;

        // Not used right now and no logic was implemented to deal with hard links. TODO.
        // ushort HardLinksCount;

		indexer_common::uint64 SizeReal;

        // Not used right now.
        // uint64 SizeAllocated;


        void SetTimestamps(const STANDARD_INFORMATION& std_info) const;


        // |parent_id| parsed from FILENAME attribute and corresponding attribute flags.
		bool SetParentID(indexer_common::uint64 parent_id, FilenameFlagsEnum flags) const;


        bool NewFlagsBetterThanUsed(FilenameFlagsEnum new_flags) const;


		bool SetName(const char16_t* name, indexer_common::ushort name_length, FilenameFlagsEnum flags) const;


		void SetFileAttributes(indexer_common::ulong file_attributes) const;


        bool SetFilenameFlags(FilenameFlagsEnum new_flags);


		std::unique_ptr<indexer_common::FileInfo> GetFinalResult();

       private:
		std::unique_ptr<indexer_common::FileInfo> fi_;
    };

} // namespace ntfs_reader