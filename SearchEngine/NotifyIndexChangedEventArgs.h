// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>
#include <vector>

#include "Macros.h"

namespace indexer {

    class FileInfo;

	// Event arguments which used when index changed event fired, to pass them to the search engine.
	// Search engine must delete old items from search result and merge new and changed items with search result.

    class NotifyIndexChangedEventArgs {

       public:
        NotifyIndexChangedEventArgs(std::vector<const FileInfo*>&& new_items, std::vector<const FileInfo*>&& old_items,
                                    std::vector<const FileInfo*>&& changed_items);

        NO_COPY(NotifyIndexChangedEventArgs)


        // Items newly added to a volume Index.

        std::vector<const FileInfo*> NewItems;


        // Items removed from a volume Index.

        std::vector<const FileInfo*> OldItems;


        // Items, which have changed some their properties (name, timestamps, parentID etc.).

        std::vector<const FileInfo*> ChangedItems;


        // String representation of the arguments.

        std::wstring ToWString() const;
    };

    typedef std::shared_ptr<NotifyIndexChangedEventArgs> pNotifyIndexChangedEventArgs;

} // namespace indexer