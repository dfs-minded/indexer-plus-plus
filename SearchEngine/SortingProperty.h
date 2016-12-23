// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <string>

namespace indexer {

    enum class SortingProperty {

        SORT_NOTSUPPORTED_COLUMNS,
        SORT_NAME,
        SORT_SIZE,
        SORT_CREATION_TIME,
        SORT_LASTACCESS_TIME,
        SORT_LASTWRITE_TIME,
        SORT_PATH,
        SORT_EXTENSION,
        SORT_TYPE
    };

    SortingProperty PropertyNameToSortingPropertyEnum(std::string prop_name);

    std::wstring SortingPropertyEnumToPropertyName(SortingProperty prop);

} // namespace indexer