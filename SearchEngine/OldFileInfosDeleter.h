// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <vector>

#include "Macros.h"

#include "Log.h"

namespace indexer_common {
	class FileInfo;
}

namespace indexer {

	// This class is used for destruction obsolete FileInfo objects when they are not needed anymore.
	// It works like a smart pointer for FileInfos.

    class OldFileInfosDeleter {
       public:
        OldFileInfosDeleter() = default;

        NO_COPY(OldFileInfosDeleter)

        ~OldFileInfosDeleter();

		void AddItemsToDelete(std::vector<const indexer_common::FileInfo*>&& file_infos);

       private:
		   static indexer_common::Log& Logger();

		   std::vector<std::vector<const indexer_common::FileInfo*>> file_infos_to_delete_;
    };

} // namespace indexer