// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <functional>

#include "LetterCaseMatching.h"
#include "SortingProperty.h"

namespace indexer {

    class FileInfo;

    typedef std::function<bool(const FileInfo* first, const FileInfo* second)> PropertyComparatorFunc;

    typedef const std::pair<int, const FileInfo*> sort_pair;
    typedef std::function<bool(sort_pair first, sort_pair second)> PairComparatorFunc;

    class FileInfoComparatorFactory {
       public:
        static PropertyComparatorFunc CreatePropertyComparator(SortingProperty sort_prop, int direction, bool match_case);

        static PairComparatorFunc CreatePairsFirstDefaultComparator(int direction);
    };

} // namespace indexer