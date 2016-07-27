#pragma once

#include "SearchResult.h"

class SearchResultObserver {

   public:
    virtual void OnNewSearchResult(pSearchResult search_result, bool isNewQuery) = 0;

    virtual ~SearchResultObserver(){};
};
