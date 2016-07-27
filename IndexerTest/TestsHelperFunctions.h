// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "SearchQuery.h"

// Since SearchQuery is immutable, need some factory for producing equal queries.
SearchQuery* CopyQuery(const SearchQuery& other);
