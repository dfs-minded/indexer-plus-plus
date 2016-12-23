// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "WindowsWrapper.h"

#include "typedefs.h"

namespace ntfs_reader {

	// Holds main properties of a volume.

    struct VolumeData {
       public:
        VolumeData();

        VolumeData(char drive_letter, const NTFS_VOLUME_DATA_BUFFER& volume_data);

        ~VolumeData() = default;

        char DriveLetter;

		indexer_common::uint BytesPerCluster;

		indexer_common::uint BytesPerSector;

        // Logical cluster number where the MFT begins.
		indexer_common::uint64 MFTStartLCN;

		indexer_common::uint MFTRecordSize;

		indexer_common::uint64 MFTSize;

		indexer_common::uint64 MFTRecordsNum;
    };

} // namespace ntfs_reader