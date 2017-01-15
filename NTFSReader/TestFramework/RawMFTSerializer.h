// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <fstream>
#include <memory>

#include "WindowsWrapper.h"

#include "macros.h"

namespace ntfs_reader {

    class RawMFTSerializer {
       public:
        NO_COPY(RawMFTSerializer)

        static RawMFTSerializer& Instance();

        void SerializeMFT(void* buffer, DWORD bytes_to_write) const;

        bool GetNtfsVolumeData(NTFS_VOLUME_DATA_BUFFER* volume_data_buff);

        bool ReadMFTFromFile(void* buffer, DWORD bytes_to_read);

       private:
        RawMFTSerializer();

        ~RawMFTSerializer();

        HANDLE raw_mft_file_;

        std::unique_ptr<std::ifstream> serialized_raw_mft_file_;
    };

} // namespace ntfs_reader