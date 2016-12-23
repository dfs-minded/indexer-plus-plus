// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "MFTReaderFactory.h"

#include <memory>

#include "CommandlineArguments.h"
#include "CompilerSymb.h"

#include "MockMFTReader.h"
#include "RawMftReader.h"
#include "WinApiMftReader.h"

namespace ntfs_reader {

	using namespace indexer_common;

    std::unique_ptr<MFTReader> MFTReaderFactory::CreateReader(char drive_letter) {
        std::unique_ptr<MFTReader> reader;

#ifdef WIN_API_MFT_READ
        reader = make_unique<WinApiMFTReader>(drive_letter);
#else
        if (!CommandlineArguments::Instance().ReplayFileInfosPath.empty())
            reader = std::make_unique<MockMFTReader>();
        else
            reader = std::make_unique<RawMFTReader>(drive_letter);
#endif
        return reader;
    }

} // namespace ntfs_reader