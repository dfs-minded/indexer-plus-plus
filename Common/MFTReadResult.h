#pragma once

#include <memory>
#include <vector>

#include "typedefs.h"

struct MFTReadResult {
   public:
    MFTReadResult() : Root(nullptr) {
    }

    // The root directory in a volume.
    FileInfo* Root;

    // All volume files. Each file stored at vector index equal to its ID.
    std::unique_ptr<std::vector<FileInfo*>> Data;
};