// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "QueryProcessor.h"

#include "SearchQuery.h"

#include "OutputFormatter.h"
#include "SearchEngine.h"

using namespace std;

QueryProcessor::QueryProcessor() : engine_(make_unique<SearchEngine>(nullptr, false)) {
}

std::vector<std::wstring> QueryProcessor::Process(const std::wstring& query, const std::wstring& format_string,
                                                  int max_files) {

    uSearchQuery pq = DeserializeQuery(query);
    auto search_res = engine_->Search(pq.release());
    OutputFormatter fmt(search_res->Files.get(), format_string, max_files);
    return fmt.Format();
}