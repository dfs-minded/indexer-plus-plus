// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "FilenameSearchQuery.h"

#include "HelperCommon.h"

namespace indexer {

    using namespace std;

    FilenameSearchQuery::FilenameSearchQuery() : NStrs(0), MinLength(0) {
    }

    FilenameSearchQuery::~FilenameSearchQuery() {
        for (size_t i = 0; i < Strs.size(); ++i)
            delete Strs[i];
    }

    unique_ptr<FilenameSearchQuery> ParseFilenameQuery(const u16string& text) {

        auto result = make_unique<FilenameSearchQuery>();

        auto parts = Helper::Split(text, u16string(reinterpret_cast<const char16_t*>(L"*?")),
                                         Helper::SplitOptions::INCLUDE_SPLITTERS);

        if (parts.size() == 0) return result;

        if (parts.size() == 1) {

            if (parts[0][0] == L'?')
                result->NChars.push_back(parts[0].size());
            else if (parts[0][0] != L'*')
                result->Strs.push_back(Helper::CopyU16StringToChar16(parts[0]));

        } else {

            size_t i = 1;
            if (parts[0][0] == '?')
                result->NChars.push_back(parts[0].size());
            else if (parts[0][0] == '*')
                result->NChars.push_back(-1);
            else {
                i = 0;
                result->NChars.push_back(0);
            }

            for (; i < parts.size(); ++i) {

                if (parts[i][0] == L'*') {
                    if (result->NChars.size() == result->Strs.size())
                        result->NChars.push_back(-1);
                    else
                        result->NChars.back() = -1;  // todo process cases ??*
                } else if (parts[i][0] == L'?') {
                    if (result->NChars.size() == result->Strs.size()) result->NChars.push_back(parts[i].size());
                } else  // no wildcards
                {
                    result->Strs.push_back(Helper::CopyU16StringToChar16(parts[i]));
                }
            }
        }

        result->NStrs = result->Strs.size();

        // Calculate minimum length of string that satisfied current query.
        result->MinLength = 0;

        for (auto i = 0; i < result->NStrs; ++i) {
            result->StrLengths.push_back(Helper::Str16Len(result->Strs[i]));
            result->MinLength += result->StrLengths[i];
        }

        return result;
    }

} // namespace indexer