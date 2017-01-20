// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <string>

#include "macros.h"

#include "IndexChangeObserver.h"
#include "NotifyIndexChangedEventArgs.h"
#include "SearchResult.h"
#include "SearchQuery.h"

namespace indexer {

	class SearchResultObserver;
	class SearchEngineImpl;

	class SearchEngine : public IndexChangeObserver {
	public:

		// When |search_mode_only| = false, SearchEngine doesn't listen to changes in file system and doesn't create
		// a dedicated thread for search. It can work only in sync mode, so only Search() method should be called.

		explicit SearchEngine(SearchResultObserver* result_observer, bool search_mode_only = false);

		NO_COPY(SearchEngine)

		~SearchEngine();


		// Main method to perform search in volumes indices with the given user search query. Executed synchronously.
		// Returns a search result: collection of FileInfo objects, satisfying the given search query.

		pSearchResult Search(indexer_common::uSearchQuery&& query) const;


		// The asynchronous variant of the Search member function.

		void SearchAsync(indexer_common::uSearchQuery&& query) const;


		// Performs sorting of the search result with the given FileInfo property and sort direction 
		// 1 for ascending order, -1 for descending.

		void Sort(std::string prop_name, int direction) const;


		virtual void OnIndexChanged(pNotifyIndexChangedEventArgs p_args) override;

	private:
		std::unique_ptr<SearchEngineImpl> p_impl_;
	};

} // namespace indexer