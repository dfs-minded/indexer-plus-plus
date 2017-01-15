// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#include <memory>

#include "FileInfo.h"
#include "FileInfoHelper.h"
#include "FileInfosFilter.h"
#include "Helpers.h"
#include "LetterCaseMatching.h"
#include "SearchQueryBuilder.h"

namespace indexer {

	using std::wstring;
	using std::vector;
	using std::unique_ptr;
	using std::make_unique;

	using indexer_common::FileInfo;
	using indexer_common::SearchQuery;

    class FileInfosFilterTest : public testing::Test {
       protected:
        void SetUp() override {

            fileinfos_string_source_ = {
                L"Z|2814749767|1407374883|4|$MFT|6|7864320|7864320|1307221673|1307221673|1307221673|",
                L"Z|1407374883|1407374883|1|.|22|0|0|1307221673|1310313261|1310313261|",
                L"Z|3629499534|1407374883|10|Winamp.lnk|32|820|4096|130727861|1307278616|1310201253|",
                L"Z|1125899906|3937594343|11|desktop.ini|32|129|24|1308220894|1308220894|1308220894|",
                L"Z|3629499534|1407374885|19|IndexerDebugLog.txt|32|22051|24576|1307272658|1307272658|1307273188|",
                L"Z|1125899906|1407374883|10|New folder|16|0|0|1310243724|1310243724|1310243724|",
            };

            for (const auto& s : fileinfos_string_source_) {
                fileinfos_test_collection_.emplace_back(indexer_common::DeserializeFileInfo(s));
            }
        }

        void TearDown() override {
        }


        // Used from the test framework serialized FileInfo objects.

        vector<wstring> fileinfos_string_source_;


        vector<unique_ptr<FileInfo>> fileinfos_test_collection_;


        // The class instance, which is the test cases target.

        FileInfosFilter filter_;


        // Accessors to |filter_| private members;

        const SearchQuery* GetFiltersInternalQuery() const {
            return filter_.query_.get();
        }

        const indexer_common::ushort* GetFiltersMatchCaseTable() const {
            return filter_.match_case_table_;
        }

        const FilenameSearchQuery* GetFiltersFilenameFilter() const {
            return filter_.filename_filter_.get();
        }

        bool PassesSizeDatesFilterCall(const FileInfo& fi) const {
            return filter_.PassesSizeDatesFilter(fi);
        }
    };

    TEST_F(FileInfosFilterTest, EverythingIsNullBeforeResetQueryCall) {

        EXPECT_EQ(GetFiltersInternalQuery(), nullptr);
        EXPECT_EQ(GetFiltersFilenameFilter(), nullptr);
        EXPECT_EQ(GetFiltersMatchCaseTable(), nullptr);
    }

    TEST_F(FileInfosFilterTest, ResetQueryDefaultQueryTest) {
		indexer_common::SearchQueryBuilder builder;
	
        filter_.ResetQuery(move(builder.Build()));

		auto u_default_query = builder.Build();
        EXPECT_TRUE(*GetFiltersInternalQuery() == *u_default_query);

		EXPECT_EQ(GetFiltersMatchCaseTable(), indexer_common::GetLowerMatchTable());
    }

    TEST_F(FileInfosFilterTest, ResetQueryCustomQueryTest) {

		indexer_common::SearchQueryBuilder builder;
		builder.SetSearchText(__L__("myfile*"))
			.SetSearchDirPath(__L__("d:/"))
			.SetMatchCase()
			.SetUseRegex()
			.SetExcludeFiles();

		builder.SetSizeFrom(2).SetSizeTo(33);
		
		builder.SetCreationTimeFrom(1344556677).SetCreationTimeTo(1366556677)
			.SetLastAccessTimeFrom(1333556677).SetLastAccessTimeTo(1333556677)
			.SetModificationTimeFrom(1333444455).SetModificationTimeTo(1333444466);

		auto custom_query = builder.Build();


		indexer_common::uSearchQuery custom_query_cpy = helpers::CopyQuery(*custom_query);

        filter_.ResetQuery(move(custom_query_cpy));

        EXPECT_TRUE(*GetFiltersInternalQuery() == *custom_query);

		EXPECT_EQ(GetFiltersMatchCaseTable(), indexer_common::GetIdentityTable());
    }

    TEST_F(FileInfosFilterTest, PassesSizeDatesFilterWithDefaultQueryTest) {
		auto u_query = indexer_common::SearchQueryBuilder().Build();

        filter_.ResetQuery(move(u_query));

        for (const auto& fi : fileinfos_test_collection_) {
            ASSERT_TRUE(PassesSizeDatesFilterCall(*fi));
        }
    }

    TEST_F(FileInfosFilterTest, DISABLED_PassesSizeDatesFilterWithCustomQueryTest) {

		indexer_common::SearchQueryBuilder builder;
		builder.SetSearchText(__L__("*"))
			.SetSearchDirPath(__L__(""))
			.SetMatchCase()
			.SetExcludeFiles();

		builder.SetSizeFrom(2).SetSizeTo(22051);

		builder.SetCreationTimeFrom(1307278616).SetCreationTimeTo(1310243724)
			.SetLastAccessTimeFrom(1310313261).SetLastAccessTimeTo(1310313261)
			.SetModificationTimeFrom(13102437249).SetModificationTimeTo(1310243724);

		auto custom_query = builder.Build();

		custom_query.reset();

		// both size and dates are not match.
        ASSERT_FALSE(PassesSizeDatesFilterCall(*fileinfos_test_collection_[0]));

        ASSERT_FALSE(PassesSizeDatesFilterCall(*fileinfos_test_collection_[1]));
        ASSERT_FALSE(PassesSizeDatesFilterCall(*fileinfos_test_collection_[2]));
        ASSERT_FALSE(PassesSizeDatesFilterCall(*fileinfos_test_collection_[3]));
        ASSERT_FALSE(PassesSizeDatesFilterCall(*fileinfos_test_collection_[4]));
    }

} // namespace indexer