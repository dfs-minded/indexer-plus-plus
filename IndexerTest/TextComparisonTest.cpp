#pragma once

#include "gtest/gtest.h"

#include <map>

#include "TextComparison.h"

using namespace std;

TEST(TextComparison, WcharLessComparatorWorks) {

    WcharLessComparator cmp;

    auto* s1 = reinterpret_cast<const char16_t*>(L".ex");

    EXPECT_FALSE(cmp(s1, s1));

    auto* s2 = reinterpret_cast<const char16_t*>(L".ex_");

    EXPECT_TRUE(cmp(s1, s2));
    EXPECT_FALSE(cmp(s2, s1));
}

TEST(TextComparison, MapUsesWcharLessComparator) {

    map<const char16_t*, int, WcharLessComparator> wchar_to_int_map;

    auto* s1 = reinterpret_cast<const char16_t*>(L".ex");
    auto* s2 = reinterpret_cast<const char16_t*>(L".ex_");

    wchar_to_int_map.emplace(s1, 0);
    EXPECT_FALSE(wchar_to_int_map.find(s1) == wchar_to_int_map.end());

    wchar_to_int_map.emplace(s1, 0);
    EXPECT_TRUE(wchar_to_int_map.size() == 1);

    wchar_to_int_map.emplace(s2, 0);
    EXPECT_FALSE(wchar_to_int_map.find(s2) == wchar_to_int_map.end());
    EXPECT_TRUE(wchar_to_int_map.size() == 2);

    wchar_to_int_map.emplace(s2, 0);
    EXPECT_TRUE(wchar_to_int_map.size() == 2);
}