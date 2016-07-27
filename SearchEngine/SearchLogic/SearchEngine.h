#pragma once

#include <string>

#include "Macros.h"
#include "SearchQuery.h"

#include "IndexChangeObserver.h"
#include "NotifyIndexChangedEventArgs.h"
#include "SearchResult.h"

class SearchResultObserver;
class SearchEngineImpl;

class SearchEngine : public IndexChangeObserver {
   public:
    SearchEngine(SearchResultObserver* resultObserver, bool listenIndexChange = true);

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
