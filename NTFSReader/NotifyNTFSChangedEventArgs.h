// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <map>
#include <set>

#include "Macros.h"
#include "typedefs.h"

// This arguments used to store and pass changes, made in the file system. The intended
// consumer is an IndexManager, which must process this changes, namely update an Index
// and fire index changed event.
// This args are accumulated result of one reading session.

namespace ntfs_reader {

    class NotifyNTFSChangedEventArgs {
       public:
        NotifyNTFSChangedEventArgs() = default;

        NO_COPY(NotifyNTFSChangedEventArgs)

        // Contains newly created files.

        std::map<uint, FileInfo*> CreatedItems;


        // Contains IDs of deleted files.

        std::set<uint> DeletedItems;


        // Contains new FileInfo objects, that could be matched to the old ones by ID.

        std::map<uint, FileInfo*> ChangedItems;


        // Returns string representation of the arguments.

        std::wstring ToWString() const;
    };

} // namespace ntfs_reader