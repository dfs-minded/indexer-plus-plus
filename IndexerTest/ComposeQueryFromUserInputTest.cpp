// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#include <memory>
#include <vector>
#include <string>
#include <limits>

#include "SearchQuery.h"

#include "UserArgsParser.h"

namespace ifind {

	using std::wstring;
	using std::vector;

	class ComposeQueryFromUserInputTest : public testing::Test {

	  protected:
		void SetUp() override {
		}

		void TearDown() override {
		}

		wstring empty_format_ = L"";
		wstring empty_output_path = L"";
	};


	TEST_F(ComposeQueryFromUserInputTest, PerciseSizeFilter) {
		// Default in bytes:
		const vector<wstring> args = {L"-size", L"20"};
		auto query = ComposeQueryFromUserInput(args, &empty_format_, &empty_output_path);
		EXPECT_EQ(20, query->SizeFrom);
		EXPECT_EQ(20, query->SizeTo);

		// Bytes specified in format 'c':
		const vector<wstring> args2 = { L"-size", L"20c" };
		auto query2 = ComposeQueryFromUserInput(args2, &empty_format_, &empty_output_path);
		EXPECT_EQ(20, query2->SizeFrom);
		EXPECT_EQ(20, query2->SizeTo);

		// Bytes specified in format 'B':
		const vector<wstring> args3 = { L"-size", L"20B" };
		auto query3 = ComposeQueryFromUserInput(args3, &empty_format_, &empty_output_path);
		EXPECT_EQ(20, query3->SizeFrom);
		EXPECT_EQ(20, query3->SizeTo);

		// Kilobytes specified in format 'k':
		const vector<wstring> args4 = { L"-size", L"20k" };
		auto query4 = ComposeQueryFromUserInput(args4, &empty_format_, &empty_output_path);
		EXPECT_EQ(20 * 1024, query4->SizeFrom);
		EXPECT_EQ(20 * 1024, query4->SizeTo);

		// Megabytes specified in format 'MB':
		const vector<wstring> args5 = { L"-size", L"20MB" };
		auto query5 = ComposeQueryFromUserInput(args5, &empty_format_, &empty_output_path);
		EXPECT_EQ(20 * 1024 * 1024, query5->SizeFrom);
		EXPECT_EQ(20 * 1024 * 1024, query5->SizeTo);

		// Gigabytes specified in format 'MB':
		const vector<wstring> args6 = { L"-size", L"20GB" };
		auto query6 = ComposeQueryFromUserInput(args6, &empty_format_, &empty_output_path);
		EXPECT_EQ(20 * 1024 * 1024 * 1024, query6->SizeFrom);
		EXPECT_EQ(20 * 1024 * 1024 * 1024, query6->SizeTo);
	}

	TEST_F(ComposeQueryFromUserInputTest, LessThenAndMoreThenSizeFilter) {
		// From 0 to 200 KB:
		const vector<wstring> args = { L"-size", L"-200KB" };
		auto query = ComposeQueryFromUserInput(args, &empty_format_, &empty_output_path);
		EXPECT_EQ(0, query->SizeFrom);
		EXPECT_EQ(200 * 1024, query->SizeTo);

		// From 200 to 300 KB:
		const vector<wstring> args2 = { L"-size", L"-300KB", L"-size", L"+200KB" };
		auto query2 = ComposeQueryFromUserInput(args2, &empty_format_, &empty_output_path);
		EXPECT_EQ(200 * 1024, query2->SizeFrom);
		EXPECT_EQ(300 * 1024, query2->SizeTo);

		// More then 2 GB:
		const vector<wstring> args3 = { L"-size", L"+3GB" };
		auto query3 = ComposeQueryFromUserInput(args3, &empty_format_, &empty_output_path);
		EXPECT_EQ(200 * 1024 * 1024 * 1024, query3->SizeFrom);
		int expected = std::numeric_limits<int>::max();
		EXPECT_EQ(expected, query3->SizeTo);
	}
	
} // namespace ifind 