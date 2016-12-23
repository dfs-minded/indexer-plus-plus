// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "FileInfo.h"

namespace ntfs_reader {

    class FileInfoObjectsSerializer {
       public:
        NO_COPY(FileInfoObjectsSerializer)

        static FileInfoObjectsSerializer& Instance();

		void SerializeFileInfoToFile(const indexer_common::FileInfo& fi) const;

		std::unique_ptr<std::vector<indexer_common::FileInfo*>> DeserializeAllFileInfos(const std::wstring& filename) const;

       private:
        FileInfoObjectsSerializer();

        ~FileInfoObjectsSerializer();

        FILE* file_infos_db_;
    };

} // namespace ntfs_reader