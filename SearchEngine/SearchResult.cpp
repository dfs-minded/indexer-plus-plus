// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchResult.h"
#include "FileInfo.h"

namespace indexer {

	SearchResult::SearchResult(std::unique_ptr<OldFileInfosDeleter> u_old_file_infos /*= nullptr*/)
		: Files(std::make_unique<std::vector<const indexer_common::FileInfo*>>()), 
		  old_file_infos_(std::move(u_old_file_infos)) {
    }

} // namespace indexer