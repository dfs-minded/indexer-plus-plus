// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <string>
#include <vector>

#include "SearchQuery.h"

namespace ifind {
	
	// Composes search query from user CMD input, parses and sets |format| and |output_path|.

    indexer_common::uSearchQuery ComposeQueryFromUserInput(
		const std::vector<std::wstring>& user_args, std::wstring* format, std::wstring* output_path);

} // namespace ifind