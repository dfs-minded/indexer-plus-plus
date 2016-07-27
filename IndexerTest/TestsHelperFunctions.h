#pragma once

#include "SearchQuery.h"

// Since SearchQuery is immutable, need some factory for producing equal queries.
SearchQuery* CopyQuery(const SearchQuery& other);
