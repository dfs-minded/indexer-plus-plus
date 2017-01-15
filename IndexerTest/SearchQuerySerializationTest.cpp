// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#include <memory>

#include "Helpers.h"
#include "SearchQueryBuilder.h"

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

		indexer_common::SearchQueryBuilder builder;

		builder.SetSearchText(text)
			.SetSearchDirPath(search_dir_path)
			.SetSizeFrom(size_from).SetSizeTo(size_to)
			.SetCreationTimeFrom(c_time_from).SetCreationTimeTo(c_time_to)
			.SetLastAccessTimeFrom(a_time_from).SetLastAccessTimeTo(a_time_to)
			.SetModificationTimeFrom(m_time_from).SetModificationTimeTo(m_time_to);

		if (match_case) builder.SetMatchCase();
		if (use_regex) builder.SetUseRegex();
		if (exclude_hidden_and_system) builder.SetExcludeHiddenAndSystem();
		if (exclude_folders) builder.SetExcludeFolders();
		if (exclude_files) builder.SetExcludeFiles();

		auto query = builder.Build();

        std::wstring serialised = SerializeQuery(*query);

		auto deserQuery = indexer_common::DeserializeQuery(serialised);

        ASSERT_TRUE(*query == *deserQuery);
    }

} // namespace indexer