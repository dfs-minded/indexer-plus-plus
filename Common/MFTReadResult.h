// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>
#include <vector>

#include "typedefs.h"

namespace indexer_common {

    struct MFTReadResult {
       public:
        MFTReadResult() : Root(nullptr) {
        }

        // The root directory in a volume.
        FileInfo* Root;

        // All volume files. Each file stored at vector index equal to its ID.
        std::unique_ptr<std::vector<FileInfo*>> Data;
    };
} // namespace indexer_common