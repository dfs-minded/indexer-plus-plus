// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#include <memory>

#include "Helpers.h"
#include "SearchQuery.h"

namespace indexer {

	// Test search query serialization and deserialization.
    TEST(SearchQuerySerializationTest, DeserializedQueryEqToOrig) {

        std::u16string text = __L__("That's * ^~..:the str@n/ge text *** &");
		std::u16string search_dir_path = __L__("C:/mydir");

        bool match_case = false;
		bool use_regex = false;

		using indexer_common::uint;

        int size_from = 102;
        int size_to = 300;

        bool exclude_hidden_and_system = true;
        bool exclude_folders = true;
        bool exclude_files = false;


		// TODO fix this test with real timestamps.
		uint c_time_from = 3309855552900976;
		uint c_time_to = 88783687468765949;

		uint a_time_from = 4409855552900976;
		uint a_time_to = 99783687468765949;

		uint m_time_from = 2209855552900976;
		uint m_time_to = 11783687468765949;

		indexer_common::SearchQuery query(text, search_dir_path, match_case, use_regex, size_from, size_to, 
			exclude_hidden_and_system, exclude_folders, exclude_files,
			c_time_from, c_time_to, a_time_from, a_time_to, m_time_from, m_time_to);

        std::wstring serialised = SerializeQuery(query);

		auto deserQuery = indexer_common::DeserializeQuery(serialised);

        ASSERT_TRUE(query == *deserQuery.get());
    }

} // namespace indexer