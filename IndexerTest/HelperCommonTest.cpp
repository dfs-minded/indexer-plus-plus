// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "gtest/gtest.h"

#include <string>

#include "HelperCommon.h"

#include "Helpers.h"

using namespace std;

TEST(HelperCommonTest, WstringToWchar) {

    const char16_t* strs[] = {__L__(""), __L__("text")};

    for (auto s : strs) {
        u16string input(s);
        char16_t* res = HelperCommon::CopyU16StringToChar16(input);
        EXPECT_EQ(u16string(s), u16string(res));
        delete[] res;
    }
}

TEST(HelperCommonTest, StringToWstring) {

    const string input[]     = {"string", "long long+-=\t\nstring"};
    const wstring expected[] = {L"string", L"long long+-=\t\nstring"};

    size_t n_inp = sizeof(input) / sizeof(input[0]);
    size_t n_exp = sizeof(expected) / sizeof(expected[0]);

    ASSERT_EQ(n_exp, n_inp);

    for (size_t i = 0; i < n_inp; ++i) {
        EXPECT_EQ(expected[i], HelperCommon::StringToWstring(input[i]));
    }
}

TEST(HelperCommonTest, Char16ToWstring) {
	const char16_t* s = reinterpret_cast<char16_t*>(L"file_name3");

	wstring expected = L"file_name3";
	wstring result = HelperCommon::Char16ToWstring(s);

	ASSERT_EQ(expected, result);
}

TEST(HelperCommonTest, SplitOnlyNonEmpty) {

    const wstring input_strings[] = {L"_=_+", L"no_splitters", L"no_splitters", L"one_type__splitters_",
                                     L"text with_ different ||_splitters"};

    const wstring input_splitters[] = {L"+=_", L"", L" ", L"_", L" _|"};

    const vector<wstring> expected[] = {{},
                                        {L"no_splitters"},
                                        {L"no_splitters"},
                                        {L"one", L"type", L"splitters"},
                                        {L"text", L"with", L"different", L"splitters"}};

    size_t n_inp       = sizeof(input_strings) / sizeof(input_strings[0]);
    size_t n_splitters = sizeof(input_splitters) / sizeof(input_splitters[0]);
    size_t n_exp       = sizeof(expected) / sizeof(expected[0]);

    ASSERT_EQ(n_exp, n_inp);
    ASSERT_EQ(n_exp, n_splitters);

    for (size_t i = 0; i < n_inp; ++i) {
        SCOPED_TRACE("Test " + to_string(i));
        EXPECT_EQ(expected[i], HelperCommon::Split(input_strings[i], input_splitters[i],
                                                   HelperCommon::SplitOptions::ONLY_NON_EMPTY));
    }
}

TEST(HelperCommonTest, SplitIncludeEmpty) {

    const wstring input_strings[] = {L"_=_+", L"no_splitters", L"no_splitters", L"one_type__splitters_",
                                     L"text with_ different ||_splitters"};

    const wstring input_splitters[] = {L"+=_", L"", L" ", L"_", L" _|"};

    const vector<wstring> expected[] = {{L"", L"", L"", L"", L""},
                                        {L"no_splitters"},
                                        {L"no_splitters"},
                                        {L"one", L"type", L"", L"splitters", L""},
                                        {L"text", L"with", L"", L"different", L"", L"", L"", L"splitters"}};

    size_t n_inp       = sizeof(input_strings) / sizeof(input_strings[0]);
    size_t n_splitters = sizeof(input_splitters) / sizeof(input_splitters[0]);
    size_t n_exp       = sizeof(expected) / sizeof(expected[0]);

    ASSERT_EQ(n_exp, n_inp);
    ASSERT_EQ(n_exp, n_splitters);

    for (size_t i = 0; i < n_inp; ++i) {
        SCOPED_TRACE("Test " + to_string(i));
        EXPECT_EQ(expected[i],
                  HelperCommon::Split(input_strings[i], input_splitters[i], HelperCommon::SplitOptions::INCLUDE_EMPTY));
    }
}

TEST(HelperCommonTest, SplitIncludeSplitters) {

    const wstring input_strings[] = {L"_==_+", L"no_splitters", L"no_splitters", L"one_type__splitters_",
                                     L"text with_ different ||_splitters"};

    const wstring input_splitters[]  = {L"+=_", L"", L" ", L"_", L" _|"};
    const vector<wstring> expected[] = {
        {L"_", L"==", L"_", L"+"},
        {L"no_splitters"},
        {L"no_splitters"},
        {L"one", L"_", L"type", L"__", L"splitters", L"_"},
        {L"text", L" ", L"with", L"_", L" ", L"different", L" ", L"||", L"_", L"splitters"}};

    size_t n_inp       = sizeof(input_strings) / sizeof(input_strings[0]);
    size_t n_splitters = sizeof(input_splitters) / sizeof(input_splitters[0]);
    size_t n_exp       = sizeof(expected) / sizeof(expected[0]);

    ASSERT_EQ(n_exp, n_inp);
    ASSERT_EQ(n_exp, n_splitters);

    for (size_t i = 0; i < n_inp; ++i) {
        SCOPED_TRACE("Test " + to_string(i));
        EXPECT_EQ(expected[i], HelperCommon::Split(input_strings[i], input_splitters[i],
                                                   HelperCommon::SplitOptions::INCLUDE_SPLITTERS));
    }
}

TEST(HelperCommonTest, IsAsciiString) {

    const wstring input[] = {L"", L"LetterAndD1g1ts", L"+-*", L"A7234íaa"};
    const bool expected[] = {true, true, true, false};

    size_t n_inp = sizeof(input) / sizeof(input[0]);
    size_t n_exp = sizeof(expected) / sizeof(expected[0]);

    ASSERT_EQ(n_exp, n_inp);

    for (size_t i = 0; i < n_inp; ++i) {
        EXPECT_EQ(expected[i], HelperCommon::IsAsciiString(input[i].c_str()));
    }
}

TEST(HelperCommonTest, GetDriveName) {

	const char16_t* input = __L__(L"ABaz0");
	const u16string expected[] = { __L__(L"A:"), __L__(L"B:"), __L__(L"a:"), __L__(L"z:"), __L__(L"0:") };
    auto n_inp               = 5;
    auto n_exp               = sizeof(expected) / sizeof(expected[0]);

    ASSERT_EQ(n_exp, n_inp);

    for (size_t i = 0; i < n_inp; ++i) {
        EXPECT_EQ(expected[i], HelperCommon::GetDriveName(input[i]));
    }
}