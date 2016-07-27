#pragma once

#include <memory>

#include "MFTReadResult.h"

// Represents an interface for reading and parsing MFT (Master File Table) of NTFS volume into
// FileInfo objects. Each FileInfo object represents one file in the NTFS volume. There is no
// difference between files and folders in NTFS.

class MFTReader {
   public:
    virtual ~MFTReader() = default;


    // Reads all MFT records for current volume and returns MFTReadResult
    // with root directory and, constructed for each MFT record, FileInfo objects list.

    virtual std::unique_ptr<MFTReadResult> ReadAllRecords() = 0;
};