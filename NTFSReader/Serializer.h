// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <string>
#include "WindowsWrapper.h"

#include "Macros.h"
#include "typedefs.h"

#include "NTFSDataStructures.h"

namespace ntfs_reader {

    class Serializer {
       public:
        Serializer();

        NO_COPY(Serializer)

        ~Serializer();

        const Serializer* Serialize(P_FILE_RECORD_HEADER record_header);

        const Serializer* Serialize(const std::wstring& data);

        const Serializer* Serialize(uint64 data);

        const Serializer* Endl();

        const Serializer* Delim();

       private:
        void WriteToFile(const std::wstring& data) const;

        FILE* mft_serialization_file_;

        static const std::wstring kDelim;
    };

} // namespace ntfs_reader