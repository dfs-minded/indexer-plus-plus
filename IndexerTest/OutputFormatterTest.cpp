// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#include "FileInfo.h"
#include "OutputFormatter.h"

#include "Helpers.h"

namespace indexer_common {
	class OutputFormatterTest : public testing::Test {
	  protected:
		void SetUp() override {
			auto* fi = new FileInfo('c');
			fi->SetName(__L__(L"my_file1"), 8);
			input.push_back(fi);

			fi = new FileInfo('c');
			fi->SetName(__L__(L"my_file2"), 8);
			input.push_back(fi);
		}

		std::vector<const FileInfo*> input;
	};

	TEST_F(OutputFormatterTest, EscapedBackslashInFormatString) {
		std::wstring format_str(L"%p\\\\%p\\\\");
		OutputFormatter formater(&input, format_str);
		auto res = formater.Format();

		EXPECT_TRUE((*res)[0] == std::wstring(L"my_file1\\my_file1\\"));
		EXPECT_TRUE((*res)[1] == std::wstring(L"my_file2\\my_file2\\"));
	}

	TEST_F(OutputFormatterTest, HorizontalTabInFormatString) {
		std::wstring format_str(L"%p\\t%p");
		OutputFormatter formater(&input, format_str);
		auto res = formater.Format();

		EXPECT_TRUE((*res)[0] == std::wstring(L"my_file1	my_file1"));
		EXPECT_TRUE((*res)[1] == std::wstring(L"my_file2	my_file2"));
	}

	TEST_F(OutputFormatterTest, OtherCharacterAfterBackslashInFormatString) {
		std::wstring format_str(L"%p\\d %p");
		OutputFormatter formater(&input, format_str);
		auto res = formater.Format();

		EXPECT_TRUE((*res)[0] == std::wstring(L"my_file1\\d my_file1"));
		EXPECT_TRUE((*res)[1] == std::wstring(L"my_file2\\d my_file2"));
	}

} // namespace indexer_common
