// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "typedefs.h"

namespace indexer_common {

    char16_t* search(const char16_t* text, const char16_t* pattern, const ushort* case_match_table);

    int compare(const char16_t* lhs, const char16_t* rhs, ushort* case_match_table);

    struct WcharLessComparator {

        bool operator()(const char16_t* lhs, const char16_t* rhs) const;
    };
} // namespace indexer_common