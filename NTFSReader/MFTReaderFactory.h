// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "Macros.h"

#include "MFTReader.h"

class MFTReaderFactory {
   public:
    NO_COPY(MFTReaderFactory)

    static std::unique_ptr<MFTReader> CreateReader(char drive_letter);
};
