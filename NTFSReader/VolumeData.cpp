#include "VolumeData.h"

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

    BytesPerCluster = static_cast<uint>(volume_data.BytesPerCluster);
    BytesPerSector  = volume_data.BytesPerSector;
    MFTStartLCN     = volume_data.MftStartLcn.QuadPart;
    MFTRecordSize   = volume_data.BytesPerFileRecordSegment;
    MFTSize         = volume_data.MftValidDataLength.QuadPart;
    MFTRecordsNum   = static_cast<uint64>(volume_data.MftValidDataLength.QuadPart / MFTRecordSize);
}
