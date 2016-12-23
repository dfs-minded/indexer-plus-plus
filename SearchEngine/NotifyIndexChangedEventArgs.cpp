// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "NotifyIndexChangedEventArgs.h"

#include <string>

#include "FileInfo.h"

namespace indexer {

	using std::wstring;
	using std::to_wstring;
	using std::vector;

	using indexer_common::FileInfo;

    NotifyIndexChangedEventArgs::NotifyIndexChangedEventArgs(vector<const FileInfo*>&& new_items,
                                                             vector<const FileInfo*>&& old_items,
                                                             vector<const FileInfo*>&& changed_items)
        : NewItems(new_items), OldItems(old_items), ChangedItems(changed_items) {
    }

    wstring NotifyIndexChangedEventArgs::ToWString() const {

        return L"IndexChangedEventArgs: New = " + to_wstring(NewItems.size()) + L"; Old = " + to_wstring(OldItems.size()) +
               L"; Changed = " + to_wstring(ChangedItems.size());
    }

} // namespace indexer