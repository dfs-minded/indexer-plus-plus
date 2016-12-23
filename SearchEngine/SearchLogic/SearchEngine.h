// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <string>

#include "Macros.h"
#include "SearchQuery.h"

#include "IndexChangeObserver.h"
#include "NotifyIndexChangedEventArgs.h"
#include "SearchResult.h"

namespace indexer {

    class SearchResultObserver;
    class SearchEngineImpl;

    class SearchEngine : public IndexChangeObserver {
       public:
        // When |search_mode_only| = false, SearchEngine doesn't listen to changes in file system and doesn't create
        // a dedicated thread for search. It can work only in sync mode, so only Search() method should be called.
        SearchEngine(SearchResultObserver* resultObserver, bool search_mode_only = false);

        NO_COPY(SearchEngine)

        ~SearchEngine();


        // It's not possible to use unique_ptr for the |query|, since C++/CLI can't correctly process them.
        // This function takes ownership of |query|.

        pSearchResult Search(SearchQuery* query) const;


        // This function takes ownership of |query|.

        void SearchAsync(SearchQuery* query) const;


        void Sort(const std::string& prop_name, int direction) const;


        virtual void OnIndexChanged(pNotifyIndexChangedEventArgs p_args) override;

       private:
        std::unique_ptr<SearchEngineImpl> p_impl_;
    };

} // namespace indexer