// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "Sorter.h"

#include <algorithm>
#include <unordered_set>

#include "AsyncLog.h"
#include "FileInfo.h"
#include "FileInfoHelper.h"
#include "HelperCommon.h"
#include "Log.h"
#include "Merger.h"
#include "OneThreadLog.h"
#include "SortingProperty.h"

using namespace std;

Sorter::Sorter(SortingProperty prop, int direction) : prop_(prop), direction_(direction) {

    GET_LOGGER
}

void Sorter::Sort(vector<const FileInfo*>* file_infos) const {

    auto input_size = file_infos->size();
    if (input_size == 0) return;

    if (prop_ == SortingProperty::SORT_PATH && input_size > kMaxFilesCanSortByPath) {
        logger_->Warning(METHOD_METADATA + L"Sorting so many files by 'Path' property is not implemented.");
        return;
    }

    TIK

        if (prop_ == SortingProperty::SORT_EXTENSION || prop_ == SortingProperty::SORT_TYPE) {
        SortByExtensionOrType(file_infos);
    }
    else {
        auto cmp = FileInfoComparatorFactory::CreatePropertyComparator(prop_, direction_, false);

        if (input_size > kMinNumberOfFileInfosToSortParallel)
            SortParallel(file_infos, cmp);
        else
            sort(file_infos->begin(), file_infos->end(), cmp);
    }

    wstring direction_str = direction_ > 0 ? L"Ascending" : L"Descending";

    TOK(METHOD_METADATA + L"Sorting " + to_wstring(file_infos->size()) + L" elements by '" +
        SortingPropertyEnumToPropertyName(prop_) + L"' in " + direction_str + L" order.");
}

void Sorter::SortParallel(vector<const FileInfo*>* file_infos, PropertyComparatorFunc cmp) const {

    logger_->Debug(METHOD_METADATA + L" called");
    auto input_size = file_infos->size();

	auto cores_num = HelperCommon::GetNumberOfProcessors();
    auto num_files_to_sort_per_thread = input_size / cores_num + 1;
    const auto& begin_iter = file_infos->begin();
    vector<vector<const FileInfo*>> sub_vectors_to_sort;

	TIK
    for (uint from = 0, to = min(input_size, num_files_to_sort_per_thread); from < input_size;) {

        sub_vectors_to_sort.push_back(vector<const FileInfo*>(begin_iter + from, begin_iter + to));

        from = to;
        to   = min(input_size, to + num_files_to_sort_per_thread);
    }
	TOK(L"Sorting: file copying " + to_wstring(input_size) + L" " + to_wstring(num_files_to_sort_per_thread));

    int sub_vectors_count = sub_vectors_to_sort.size();

#pragma omp parallel for
    for (auto i = 0; i < sub_vectors_count; ++i) {
        sort(sub_vectors_to_sort[i].begin(), sub_vectors_to_sort[i].end(), cmp);
    }
	TOK(L"Sorting: parallel sort");

    unordered_set<const FileInfo*> empty_do_not_include;

    for (int step = 1; step < sub_vectors_count; step += step) {

#pragma omp parallel for
        for (int j = 0; j < (sub_vectors_count - step); j += 2 * step) {

            auto u_merge_res = Merger::MergeWithMainCollection(sub_vectors_to_sort[j], empty_do_not_include,
                                                               sub_vectors_to_sort[j + step], cmp);
            sub_vectors_to_sort[j] = move(*u_merge_res);			
        }
		TOK(L"Sorting: merge  " + to_wstring(step));
    }

    file_infos->swap(sub_vectors_to_sort[0]);
    sub_vectors_to_sort.clear();
	TOK(L"Sorting: finished");
}

PropertyComparatorFunc Sorter::GetCurrentPropertyComparator() const {
    return FileInfoComparatorFactory::CreatePropertyComparator(prop_, direction_, false);
}


typedef const pair<int, const FileInfo*> sort_pair;

struct PairIntComparator {

    bool operator()(sort_pair& lhs, sort_pair& rhs) const {

        bool is_dir1 = lhs.second->IsDirectory();
        bool is_dir2 = rhs.second->IsDirectory();
        if (is_dir1 != is_dir2) return is_dir1 > is_dir2;

        return lhs.first < rhs.first;
    }
};


void Sorter::ResetSortingProperties(SortingProperty prop, int direction) {
    prop_      = prop;
    direction_ = direction;
}

void Sorter::SortByExtensionOrType(vector<const FileInfo*>* file_infos) const {

    auto input_size = file_infos->size();

    vector<pair<int, const FileInfo*>> sort_prop_to_fi_pairs(input_size);
    // Form pairs to sort <int sorting_property, const FileInfo* fi>.

    if (prop_ == SortingProperty::SORT_EXTENSION) {

        vector<const char16_t*> ext_cache;  // To prevent retrieving |file_infos| extensions extra times.
        auto& sort_prop_to_int = FileInfoHelper::GetDistinctOrderedExtensions(*file_infos, &ext_cache);

        for (size_t i = 0; i < input_size; ++i) {
            if (sort_prop_to_int.find(ext_cache[i]) == sort_prop_to_int.end()) WriteToOutput("Fuck");

            sort_prop_to_fi_pairs[i].first  = sort_prop_to_int.at(ext_cache[i]);
            sort_prop_to_fi_pairs[i].second = (*file_infos)[i];
        }

    } else if (prop_ == SortingProperty::SORT_TYPE) {

        vector<const char16_t*> types_cache;  // To prevent retrieving |file_infos| extensions extra times.

        auto& sort_prop_to_int = FileInfoHelper::GetDistinctOrderedTypes(*file_infos, &types_cache);

        for (size_t i = 0; i < input_size; ++i) {
            sort_prop_to_fi_pairs[i].first  = sort_prop_to_int.at(types_cache[i]);
            sort_prop_to_fi_pairs[i].second = (*file_infos)[i];
        }
    }

    auto pred = FileInfoComparatorFactory::CreatePairsFirstDefaultComparator(direction_);

    // auto pred = [direction](sort_pair lhs, sort_pair rhs)
    //{
    //	bool is_dir1 = lhs.second->IsDirectory();
    //	bool is_dir2 = rhs.second->IsDirectory();
    //	if (is_dir1 != is_dir2) return is_dir1 > is_dir2;

    //	return  direction > 0 ?
    //		lhs.first < rhs.first :
    //		lhs.first > rhs.first;
    //};

    sort(sort_prop_to_fi_pairs.begin(), sort_prop_to_fi_pairs.end(), /*PairIntComparator()*/ pred);

    // Copy result back.
    for (size_t i = 0; i < input_size; ++i) {
        (*file_infos)[i] = sort_prop_to_fi_pairs[i].second;
    }
}
