#pragma once

#include <memory>
#include <vector>

#include "ConnectionManager.h"
#include "Macros.h"
#include "SearchResultObserver.h"

class SearchEngine;

class QueryProcessor : public IQueryProcessor {

   public:
    QueryProcessor();

    NO_COPY(QueryProcessor)

    virtual std::vector<std::wstring> Process(const std::wstring& query, const std::wstring& format_string,
                                              int max_files) override;

   private:
    std::unique_ptr<SearchEngine> engine_;
};