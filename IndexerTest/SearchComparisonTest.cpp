// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#include "Helpers.h"
#include "LetterCaseMatching.h"
#include "TextComparison.h"

// Tests comp of two empty strings.
TEST(SearchComparison, HandlesEmptyInput) {
    ushort* table = GetLowerMatchTable();
    int res = compare(__L__(""), __L__(""), table);
    EXPECT_EQ(res, 0);
}

// Tests comp of two equal number strings.
TEST(SearchComparison, HandlesEqualNumbersInput) {
    ushort* table = GetLowerMatchTable();
    int res = compare(__L__(L"09"), __L__(L"09"), table);
    EXPECT_EQ(res, 0);
}

// Tests comp of two not equal number strings.
TEST(SearchComparison, HandlesNotEqualNumbersInput) {
    ushort* table = GetLowerMatchTable();
    int res = compare(__L__(L"123"), __L__(L"01"), table);
    EXPECT_EQ(res, 1);
}

// Tests comp of two equal strings, case insensitive.
TEST(SearchComparison, HandlesEqualCaseInsensitiveInput) {
    ushort* table = GetLowerMatchTable();
    int res = compare(__L__(L"aBcD"), __L__(L"AbCd"), table);
    EXPECT_EQ(res, 0);
}

// Tests comp of two equal strings, case sensitive. Lower is second.
TEST(SearchComparison, HandlesEqualCaseSensitiveInput) {
    ushort* table = GetIdentityTable();
    int res = compare(__L__(L"aB"), __L__(L"Ab"), table);
    EXPECT_EQ(res, 1);
}

// Tests comp of two strings, left is first. Case insensitive.
TEST(SearchComparison, LeftIsGreaterCaseInsensitiveInput) {
    ushort* table = GetLowerMatchTable();
    int res = compare(__L__(L"AB"), __L__(L"aca"), table);
    EXPECT_EQ(res, -1);
}

// Tests comp of two strings, left is first. Case sensitive.
TEST(SearchComparison, RightIsFirstCaseSensitiveInput) {
    ushort* table = GetIdentityTable();
    int res = compare(__L__(L"AB"), __L__(L"aca"), table);
    EXPECT_EQ(res, -1);
}

// Tests comp of two Cyrillic strings, right is first. Case insensitive.
TEST(SearchComparison, HandlesCyrillicRightIsFirstrCaseInsensitiveInput) {
    ushort* table = GetLowerMatchTable();
    int res = compare(__L__(L"вася"), __L__(L"ВАНЮХА"), table);
    EXPECT_EQ(res, 1);
}

// Tests comp of two umlauts strings, left is first. Case insensitive.
TEST(SearchComparison, HandlesUmlautLeftIsFirstCaseInsensitiveInput) {
    ushort* table = GetLowerMatchTable();
    int res = compare(__L__(L"Müa"), __L__(L"MÜbc"), table);
    EXPECT_EQ(res, -1);
}

// Tests comp of two umlauts strings, left is first. Case sensitive.
TEST(SearchComparison, HandlesUmlautLeftIsFirstrCaseSensitiveInput) {
    ushort* table = GetIdentityTable();
    int res = compare(__L__(L"München"), __L__(L"MÜnchen"), table);
    EXPECT_EQ(res, 1);
}