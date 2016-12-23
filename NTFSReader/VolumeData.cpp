// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "VolumeData.h"

#include <string>

#include "../Common/Helper.h"

namespace ntfs_reader {

    VolumeData::VolumeData()
        : DriveLetter(' '),
          BytesPerCluster(0),
          BytesPerSector(0),
          MFTStartLCN(0),
          MFTRecordSize(0),
          MFTSize(0),
          MFTRecordsNum(0) {
    }

    VolumeData::VolumeData(char drive_letter, const NTFS_VOLUME_DATA_BUFFER& volume_data) : DriveLetter(drive_letter) {

		BytesPerCluster = static_cast<indexer_common::uint>(volume_data.BytesPerCluster);
        BytesPerSector  = volume_data.BytesPerSector;
        MFTStartLCN     = volume_data.MftStartLcn.QuadPart;
        MFTRecordSize   = volume_data.BytesPerFileRecordSegment;
        MFTSize         = volume_data.MftValidDataLength.QuadPart;
		MFTRecordsNum = static_cast<indexer_common::uint64>(volume_data.MftValidDataLength.QuadPart / MFTRecordSize);

		indexer_common::WriteToOutput(L"MFTSize: " + std::to_wstring(MFTSize) + L" MFTRecordsNum: " + std::to_wstring(MFTRecordsNum));
    }

} // namespace ntfs_reader