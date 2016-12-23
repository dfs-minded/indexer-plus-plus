// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <unordered_set>

#include "FileInfoComparatorFactory.h"

namespace indexer_common { class FileInfo; }

namespace indexer {

    class Merger {
       public:
		static std::unique_ptr<std::vector<const indexer_common::FileInfo*>> MergeWithMainCollection(
			const std::vector<const indexer_common::FileInfo*>& source_collection,
			const std::unordered_set<const indexer_common::FileInfo*>& do_not_include,
			const std::vector<const indexer_common::FileInfo*>& items_to_include,
            const PropertyComparatorFunc cmp);
    };

} // namespace indexer