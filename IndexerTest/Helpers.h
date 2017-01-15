// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "SearchQuery.h"

namespace indexer {

	namespace helpers {

		// A macros for defining constants of type char16_t*. Todo: remove when Visual Studio will support u"" macros.

		#define __L__(str) reinterpret_cast<const char16_t*>(str)

		// Since SearchQuery is immutable, need some factory for producing equal queries.

		indexer_common::uSearchQuery CopyQuery(const indexer_common::SearchQuery& other);
		
	} //namespace helpers

} // namespace indexer