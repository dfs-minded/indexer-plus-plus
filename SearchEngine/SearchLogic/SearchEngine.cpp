// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchEngine.h"

#include "SearchQuery.h"

#include "SearchEngineImpl.h"

namespace indexer {

	using namespace indexer_common;

    SearchEngine::SearchEngine(SearchResultObserver* result_observer, bool search_mode_only /*= false by default*/) :
		p_impl_ { std::make_unique<SearchEngineImpl>(static_cast<SearchEngine*>(this), result_observer, search_mode_only) } {
    }

	// If there is no explicit destructor implementation, C++/CLI treats SearchEngine as an incomplete type.
    SearchEngine::~SearchEngine() {
    }

    pSearchResult SearchEngine::Search(SearchQuery* query) const {
        return p_impl_->Search(uSearchQuery(query));
    }

    void SearchEngine::SearchAsync(SearchQuery* query) const {
        p_impl_->SearchAsync(uSearchQuery(query));
    }

    void SearchEngine::Sort(const std::string& prop_name, int direction) const {
        p_impl_->Sort(prop_name, direction);
    }

    void SearchEngine::OnIndexChanged(pNotifyIndexChangedEventArgs p_args) {
        p_impl_->OnIndexChanged(p_args);
    }

} // namespace indexer