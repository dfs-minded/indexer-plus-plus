#include "Merger.h"

using namespace std;

// TODO think about interface and correct args naming.

unique_ptr<vector<const FileInfo*>> Merger::MergeWithMainCollection(
    const vector<const FileInfo*>& from, const unordered_set<const FileInfo*>& do_not_include,
    const vector<const FileInfo*>& items_to_include, const PropertyComparatorFunc cmp) {

    // Merge filtered and sorted new and changed items with tmp search result, considering items, which we do not want
    // to include in search result.

    auto merged = make_unique<vector<const FileInfo*>>();

    size_t i = 0, j = 0;
    for (; i < from.size() && j < items_to_include.size();) {

        if (do_not_include.count(from[i]) > 0) {
            ++i;
            continue;
        }

        if (cmp(from[i], items_to_include[j])) {
            merged->push_back(from[i]);
            ++i;
        } else {
            merged->push_back(items_to_include[j]);
            ++j;
        }
    }

    for (; i < from.size(); ++i) {

        if (do_not_include.count(from[i]) == 0) merged->push_back(from[i]);
    }

    for (; j < items_to_include.size(); ++j) {
        merged->push_back(items_to_include[j]);
    }

    return merged;
}