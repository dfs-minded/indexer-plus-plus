#pragma once

#include <unordered_set>

#include "FileInfoComparatorFactory.h"

class FileInfo;

class Merger {
   public:
    static std::unique_ptr<std::vector<const FileInfo*>> MergeWithMainCollection(
        const std::vector<const FileInfo*>& source_collection,
        const std::unordered_set<const FileInfo*>& do_not_include, const std::vector<const FileInfo*>& items_to_include,
        const PropertyComparatorFunc cmp);
};
