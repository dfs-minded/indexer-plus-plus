// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

// I did not figure out why, but because of "Windows.h" header and related definitions of min and max template
// functions, RE2 fails to compile stringpiece class.
#undef min
#undef max
#undef std::min
#undef std::max
#include "re2.h"

#include "FileInfo.h"
#include "FilenameSearchQuery.h"
#include "Macros.h"
#include "SearchQuery.h"

class FileInfosFilterTest;

// Provides all FileInfo objects filtering logic with the given search query.

class FileInfosFilter {
    friend class FileInfosFilterTest;

   public:
    FileInfosFilter();

    NO_COPY(FileInfosFilter)

    ~FileInfosFilter() = default;


    // Resets the underlying search query.

    void ResetQuery(std::unique_ptr<SearchQuery> last_query);


    // Used during the files tree traversal. Checks if the file |fi| passes all |query_| filters.

    bool PassesAllQueryFilters(const FileInfo& fi);


    // Used during the files tree traversal. We do not want to traverse hidden directories (because their content
    // is also hidden files) if |query_| contains exclude hidden filter set to true.

    bool TraverseCurrentDir(const FileInfo& dir) const;


    // Returns true if the FileInfo |fi| passes |query_| search in particular directory restriction.
    // Used for checking one single file.

    bool FilePassesSearchDirPathFilter(const FileInfo& fi) const;


    // Returns true if the FileInfo |fi| passes |query_| exclude hidden files and directories restriction.
    // Used for checking one single file.

    bool FilePassesExcludeHiddenAndSystemFilter(const FileInfo& fi) const;


    // Returns filtered items collection.

    std::vector<const FileInfo*> FilterFiles(const std::vector<const FileInfo*>& input);


    bool SearchInDirectorySpecified() const;


    // Returns FileInfo object, which represents search directory, specified in |query_| filter.

    const FileInfo* GetSearchDirectory() const;

   private:
    bool PassesFilterByFilename(const FileInfo& fi) const;

    bool PassesSizeDatesFilter(const FileInfo& fi) const;

    bool PassesAttributesFilter(const FileInfo& fi) const;


    // Copy of the query received from SE client for using in the SearchEngine worker thread.

    std::unique_ptr<SearchQuery> query_;


    // It contains parsed data from |cached_last_query_| for search in filenames.

    std::unique_ptr<FilenameSearchQuery> filename_filter_;

    std::unique_ptr<re2::RE2> re_;

    const int kBufferSize = 1000;

    char* buffer_;

    ushort* match_case_table_;  // TODO use from MFT.
};
