#pragma once

#include "WindowsWrapper.h"

#include "typedefs.h"

struct VolumeData {
   public:
    VolumeData();

    VolumeData(char drive_letter, const NTFS_VOLUME_DATA_BUFFER& volume_data);

    ~VolumeData() = default;

    char DriveLetter;

    uint BytesPerCluster;

    uint BytesPerSector;

    // Logical cluster number where the MFT begins.
    uint64 MFTStartLCN;

    uint MFTRecordSize;

    uint64 MFTSize;

    uint64 MFTRecordsNum;
};
