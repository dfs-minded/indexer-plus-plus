#pragma once

#include <memory>
#include <vector>

#include "Macros.h"

struct FilenameSearchQuery {
   public:
    FilenameSearchQuery();

    NO_COPY(FilenameSearchQuery)

    ~FilenameSearchQuery();

    std::vector<int> NChars;

    std::vector<char16_t*> Strs;

    std::vector<int> StrLengths;

    int NStrs;

    int MinLength;
};

std::unique_ptr<FilenameSearchQuery> ParseFilenameQuery(const std::u16string& text);
