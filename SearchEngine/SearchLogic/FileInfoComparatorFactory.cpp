// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "FileInfoComparatorFactory.h"

#include <cassert>
#include <functional>

#include "FileInfo.h"
#include "FileInfoHelper.h"

#include "TextComparison.h"

namespace indexer {

    using namespace std;

#define SORT_DIRS_FIRST                   \
        bool is_dir1 = first->IsDirectory();  \
        bool is_dir2 = second->IsDirectory(); \
        if (is_dir1 != is_dir2) return is_dir1 > is_dir2;

    PropertyComparatorFunc FileInfoComparatorFactory::CreatePropertyComparator(SortingProperty sort_prop, int direction,
                                                                               bool match_case) {

        auto* letter_match_table = match_case ? GetIdentityTable() : GetLowerMatchTable();

        PropertyComparatorFunc less_cmp, greater_cmp;

        switch (sort_prop) {
            case SortingProperty::SORT_NAME: {

                less_cmp = [letter_match_table](const FileInfo* first, const FileInfo* second) {
                    SORT_DIRS_FIRST
                    return compare(first->GetName(), second->GetName(), letter_match_table) < 0;
                };

                break;
            }
            case SortingProperty::SORT_PATH: {

                less_cmp = [letter_match_table](const FileInfo* first, const FileInfo* second) {
                    SORT_DIRS_FIRST

                    // TODO: more effective path comparison w/o creating full path.
                    auto u_path1 = FileInfoHelper::GetPath(*first, false);
                    auto u_path2 = FileInfoHelper::GetPath(*second, false);
                    return compare(u_path1.get(), u_path2.get(), letter_match_table) < 0;
                };

                break;
            }
            case SortingProperty::SORT_EXTENSION: {

                less_cmp = [](const FileInfo* first, const FileInfo* second) {
                    SORT_DIRS_FIRST
                    auto ext1 = FileInfoHelper::GetExtension(*first);
                    auto ext2 = FileInfoHelper::GetExtension(*second);

                    assert(ext1);
                    assert(ext2);

                    WcharLessComparator wchar_less;
                    return wchar_less(ext1, ext2);
                };

                break;
            }
            case SortingProperty::SORT_TYPE: {

                less_cmp = [](const FileInfo* first, const FileInfo* second) {
                    SORT_DIRS_FIRST
                    const auto* type1 = FileInfoHelper::GetType(*first);
                    const auto* type2 = FileInfoHelper::GetType(*second);

                    assert(type1);
                    assert(type2);

                    WcharLessComparator wchar_less;
                    return wchar_less(type1, type2);
                };

                break;
            }
            case SortingProperty::SORT_SIZE: {

                less_cmp = [](const FileInfo* first, const FileInfo* second) {
                    SORT_DIRS_FIRST
                    return first->SizeReal < second->SizeReal;
                };

                break;
            }
            case SortingProperty::SORT_CREATION_TIME: {

                less_cmp = [](const FileInfo* first, const FileInfo* second) {
                    SORT_DIRS_FIRST
                    return first->CreationTime < second->CreationTime;
                };

                break;
            }
            case SortingProperty::SORT_LASTACCESS_TIME: {

                less_cmp = [](const FileInfo* first, const FileInfo* second) {
                    SORT_DIRS_FIRST
                    return first->LastAccessTime < second->LastAccessTime;
                };

                break;
            }
            case SortingProperty::SORT_LASTWRITE_TIME: {

                less_cmp = [](const FileInfo* first, const FileInfo* second) {
                    SORT_DIRS_FIRST
                    return first->LastWriteTime < second->LastWriteTime;
                };

                break;
            }

            default:
                assert(false);  // All properties must be handled.
        }

        greater_cmp = [less_cmp](const FileInfo* first, const FileInfo* second) {
            SORT_DIRS_FIRST
            return less_cmp(second, first);  // Switched places of the first and second args.
        };

        return direction == 1 ? less_cmp : greater_cmp;
    }

    PairComparatorFunc FileInfoComparatorFactory::CreatePairsFirstDefaultComparator(int direction) {

        PairComparatorFunc less_cmp = [](sort_pair lhs, sort_pair rhs) {
            const FileInfo* first  = lhs.second;
            const FileInfo* second = rhs.second;
            SORT_DIRS_FIRST

            return lhs.first < rhs.first;
        };

        PairComparatorFunc greater_cmp = [](sort_pair lhs, sort_pair rhs) {
            const FileInfo* first  = lhs.second;
            const FileInfo* second = rhs.second;
            SORT_DIRS_FIRST

            return lhs.first > rhs.first;
        };

        return direction == 1 ? less_cmp : greater_cmp;
    }

} // namespace indexer