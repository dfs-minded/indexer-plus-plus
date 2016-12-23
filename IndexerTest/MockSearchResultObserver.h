// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "SearchResultObserver.h"

namespace indexer {

    class MockSearchResultObserver : public SearchResultObserver {

       public:
        void OnNewSearchResult(pSearchResult search_result, bool isNewQuery) {
            LastResult = search_result;
        }

        pSearchResult LastResult;
    };

} // namespace indexer