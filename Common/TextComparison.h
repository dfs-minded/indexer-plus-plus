#pragma once

#include "typedefs.h"

char16_t* search(const char16_t* text, const char16_t* pattern, const ushort* case_match_table);

int compare(const char16_t* lhs, const char16_t* rhs, ushort* case_match_table);

struct WcharLessComparator {

    bool operator()(const char16_t* lhs, const char16_t* rhs) const;
};