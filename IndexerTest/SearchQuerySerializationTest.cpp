// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "gtest/gtest.h"

#include <memory>

#include "SearchQuery.h"
#include "Helpers.h"

using namespace std;

// Test search query serialization and deserialization.

TEST(SearchQuerySerializationTest, DeserializedQueryEqToOrig) {

    u16string text            = __L__("That's * ^~..:the str@n/ge text *** &");
	u16string search_dir_path = __L__("C:/mydir");
    bool match_case         = false;

    uint64 size_from = 102;
    uint64 size_to   = 300;

    bool exclude_hidden_and_system = true;
    bool exclude_folders           = true;
    bool exclude_files             = false;

    uint64 c_time_from = 3309855552900976;
    uint64 c_time_to   = 88783687468765949;

    uint64 a_time_from = 4409855552900976;
    uint64 a_time_to   = 99783687468765949;

    uint64 m_time_from = 2209855552900976;
    uint64 m_time_to   = 11783687468765949;

    SearchQuery query(text, search_dir_path, match_case, size_from, size_to, exclude_hidden_and_system, exclude_folders,
                      exclude_files, c_time_from, c_time_to, a_time_from, a_time_to, m_time_from, m_time_to);

    wstring serialised = SerializeQuery(query);

    auto deserQuery = DeserializeQuery(serialised);

    ASSERT_TRUE(query == *deserQuery.get());
}