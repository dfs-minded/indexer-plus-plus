#include "SearchEngine.h"

#include "SearchEngineImpl.h"

SearchEngine::SearchEngine(SearchResultObserver* resultObserver, bool listenIndexChange /*= true by default*/) {

    p_impl_ = std::make_unique<SearchEngineImpl>(static_cast<SearchEngine*>(this), resultObserver, listenIndexChange);
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