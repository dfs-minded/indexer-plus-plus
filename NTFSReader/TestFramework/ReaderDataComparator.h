// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <vector>

#include "typedefs.h"

namespace indexer_common { class FileInfo; }

namespace ntfs_reader {

    struct MFTReadResult;

    class ReaderDataComparator {
       public:
		void Compare(const std::vector<indexer_common::FileInfo*>& lhs, const indexer_common::FileInfo& lhs_root, const std::wstring& lhs_name,
		const std::vector<indexer_common::FileInfo*>& rhs, const indexer_common::FileInfo& rhs_root, const std::wstring& rhs_name);

       private:
		void Compare(const std::vector<indexer_common::FileInfo*>& lhs, const std::vector<indexer_common::FileInfo*>& rhs);

		static bool Compare(const indexer_common::FileInfo& lhs, const indexer_common::FileInfo& rhs);

		static std::wstring SerializeOnlyWhatIsDiffer(const indexer_common::FileInfo& lhs, const indexer_common::FileInfo& rhs);

        std::wstring result_;
        std::wstring lhs_name_;
        std::wstring rhs_name_;
    };

} // namespace ntfs_reader