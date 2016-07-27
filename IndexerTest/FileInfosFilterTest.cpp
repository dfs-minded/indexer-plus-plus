#include "gtest/gtest.h"

#include <memory>

#include "FileInfo.h"
#include "FileInfoHelper.h"
#include "FileInfosFilter.h"
#include "Helpers.h"
#include "LetterCaseMatching.h"
#include "SearchQuery.h"

#include "TestsHelperFunctions.h"

using namespace std;

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
            fileinfos_test_collection_.push_back(DeserializeFileInfo(s));
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

    const SearchQuery* query_get() const {
        return filter_.query_.get();
    }

    const ushort* match_case_table_get() const {
        return filter_.match_case_table_;
    }

    const FilenameSearchQuery* filename_filter_get() const {
        return filter_.filename_filter_.get();
    }

    bool PassesSizeDatesFilterCall(const FileInfo& fi) {
        return filter_.PassesSizeDatesFilter(fi);
    }
};

TEST_F(FileInfosFilterTest, EverythingIsNullBeforeResetQueryCall) {

    EXPECT_EQ(query_get(), nullptr);
    EXPECT_EQ(filename_filter_get(), nullptr);
    EXPECT_EQ(match_case_table_get(), nullptr);
}

TEST_F(FileInfosFilterTest, ResetQueryDefaultQueryTest) {

    auto u_default_query = make_unique<SearchQuery>();
    filter_.ResetQuery(move(u_default_query));

    SearchQuery default_query;
    EXPECT_TRUE(*query_get() == default_query);

    EXPECT_EQ(match_case_table_get(), GetLowerMatchTable());
}

TEST_F(FileInfosFilterTest, ResetQueryCustomQueryTest) {

    SearchQuery custom_query(__L__("myfile*"), __L__("d:/"), true, 2, 33, false, false, true, 1344556677, 1366556677, 1333556677,
                             1333556677, 1333444455, 1333444466);

    unique_ptr<SearchQuery> u_custom_query;
    u_custom_query.reset(CopyQuery(custom_query));

    filter_.ResetQuery(move(u_custom_query));

    EXPECT_TRUE(*query_get() == custom_query);

    EXPECT_EQ(match_case_table_get(), GetIdentityTable());
}

TEST_F(FileInfosFilterTest, PassesSizeDatesFilterWithDefaultQueryTest) {
    auto u_query = make_unique<SearchQuery>();

    filter_.ResetQuery(move(u_query));

    for (const auto& fi : fileinfos_test_collection_) {
        ASSERT_TRUE(PassesSizeDatesFilterCall(*fi));
    }
}

TEST_F(FileInfosFilterTest, DISABLED_PassesSizeDatesFilterWithCustomQueryTest) {

    SearchQuery custom_query(__L__("*"), __L__(""), true, /*size from*/ 2, /*size to*/ 22051, false, false, true,
                             /*times: from and to, access, creation and modification respectively*/
                             1307278616, 1310243724, 1310313261, 1310313261, 13102437249, 1310243724);

    ASSERT_FALSE(PassesSizeDatesFilterCall(*fileinfos_test_collection_[0]));  // both size and dates not passed.
    ASSERT_FALSE(PassesSizeDatesFilterCall(*fileinfos_test_collection_[1]));
    ASSERT_FALSE(PassesSizeDatesFilterCall(*fileinfos_test_collection_[2]));
    ASSERT_FALSE(PassesSizeDatesFilterCall(*fileinfos_test_collection_[3]));
    ASSERT_FALSE(PassesSizeDatesFilterCall(*fileinfos_test_collection_[4]));
}
