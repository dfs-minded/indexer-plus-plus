#pragma once

#include <functional>

#include "LetterCaseMatching.h"
#include "SortingProperty.h"

class FileInfo;

typedef std::function<bool(const FileInfo* first, const FileInfo* second)> PropertyComparatorFunc;

typedef const std::pair<int, const FileInfo*> sort_pair;
typedef std::function<bool(sort_pair first, sort_pair second)> PairComparatorFunc;

class FileInfoComparatorFactory {
   public:
    static PropertyComparatorFunc CreatePropertyComparator(SortingProperty sort_prop, int direction, bool match_case);

    static PairComparatorFunc CreatePairsFirstDefaultComparator(int direction);
};
