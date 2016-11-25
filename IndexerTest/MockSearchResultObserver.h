// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "SearchResultObserver.h"

class MockSearchResultObserver : public SearchResultObserver {

public:
	void OnNewSearchResult(pSearchResult search_result, bool isNewQuery) {
		int usc = search_result.use_count();
		LastResult = search_result;
		int usc2 = search_result.use_count();
		int usc3 = LastResult.use_count();
	}
	
	pSearchResult LastResult;
};