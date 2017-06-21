// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchEngine.h"

#include "SearchQuery.h"

#include "SearchEngineImpl.h"

namespace indexer {

	using namespace indexer_common;

	SearchEngine::SearchEngine(SearchResultObserver* result_observer, bool search_mode_only /*= false by default*/)
		: p_impl_{std::make_unique<SearchEngineImpl>(this, result_observer, search_mode_only)} {
	}

	SearchEngine::~SearchEngine() = default;

	pSearchResult SearchEngine::Search(uSearchQuery&& query) const {
		return p_impl_->Search(std::move(query));
	}

	void SearchEngine::SearchAsync(uSearchQuery&& query) const {
		p_impl_->SearchAsync(std::move(query));
	}

	void SearchEngine::Sort(std::string prop_name, int direction) const {
		p_impl_->Sort(prop_name, direction);
	}

	void SearchEngine::OnIndexChanged(pNotifyIndexChangedEventArgs p_args) {
		p_impl_->OnIndexChanged(p_args);
	}

}  // namespace indexer