// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "SearchResult.h"

class SearchResultObserver {

   public:
    virtual void OnNewSearchResult(pSearchResult search_result, bool isNewQuery) = 0;

    virtual ~SearchResultObserver(){};
};
