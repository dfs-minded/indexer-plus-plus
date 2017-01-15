// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "QueryProcessor.h"

#include <memory>

#include "OutputFormatter.h"
#include "SearchEngine.h"
#include "SearchQueryBuilder.h"

namespace indexer {

    using std::wstring;

	using namespace indexer_common;

        std::unique_ptr<std::vector<wstring>> QueryProcessor::Process(
			const wstring& query_string, const wstring& format_string, int max_files) {

            auto engine(std::make_unique<SearchEngine>(nullptr, true));

            auto query = DeserializeQuery(query_string);

            auto search_res = engine->Search(std::move(query));

            OutputFormatter fmt(search_res->Files.get(), format_string, max_files);
            return fmt.Format();
    }

} // namespace indexer