// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "macros.h"

#include "MFTReadResult.h"
#include "MFTReader.h"

namespace ntfs_reader {

    class MockMFTReader : public MFTReader {
       public:
        NO_COPY(MockMFTReader)

        MockMFTReader() = default;

		virtual std::unique_ptr<indexer_common::MFTReadResult> ReadAllRecords() override;
    };

} // namespace ntfs_reader