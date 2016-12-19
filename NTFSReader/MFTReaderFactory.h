// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "Macros.h"

#include "MFTReader.h"

// Depending on cmd arguments, creates mock or real MFT reader. It also can create WinAPI MFT reader (if the
// corresponding compilation symbol defined) - one more option for testing purposes to compare results, which
// retrieved with raw data parsing.

class MFTReaderFactory {
   public:
    NO_COPY(MFTReaderFactory)

    static std::unique_ptr<MFTReader> CreateReader(char drive_letter);
};
