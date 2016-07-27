// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>
#include <vector>

#include "FileInfo.h"
#include "Macros.h"
#include "OldFileInfosDeleter.h"

struct SearchResult {

   public:
    explicit SearchResult(std::unique_ptr<OldFileInfosDeleter> u_old_file_infos = nullptr);

    NO_COPY(SearchResult)

    ~SearchResult() = default;

    // Files which are the result of the search.
    std::unique_ptr<std::vector<const FileInfo*>> Files;

   private:
    std::unique_ptr<OldFileInfosDeleter> old_file_infos_;
};

typedef std::shared_ptr<const SearchResult> pSearchResult;
