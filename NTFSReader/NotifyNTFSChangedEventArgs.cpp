// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "NotifyNTFSChangedEventArgs.h"

#include <string>

using namespace std;

wstring NotifyNTFSChangedEventArgs::ToWString() const {
    return wstring(L" NTFSChangedEventArgs: ") + L"Created = " + to_wstring(CreatedItems.size()) + L"; Deleted = " +
           to_wstring(DeletedItems.size()) + L"; Changed = " + to_wstring(ChangedItems.size());
}
