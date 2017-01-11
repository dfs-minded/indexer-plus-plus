// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>
#include <vector>

#include "macros.h"
#include "typedefs.h"

STL_EXTERN EXPORT std::wstring;

namespace indexer_common {

    class FileInfo;

    // From the input FileInfo objects list and formatting string, creates serialized into strings (in a given format)
    // FileInfos output result.
    class EXPORT OutputFormatter {
       public:
        OutputFormatter(std::vector<const FileInfo*>* const input, std::wstring format_string = std::wstring(L""),
                        size_t max_files = -1);

        NO_COPY(OutputFormatter)

        std::unique_ptr<std::vector<std::wstring>> Format();

       private:
        void ReplaceSmallLettersInFormatToUppercase();

        bool DefaultFormat();

        void WriteDefaultFormat(std::vector<std::wstring>* output);

        void WriteCustomUserFormat(std::vector<std::wstring>* output);

        bool DateTypeIsSet() const;

        bool DateTimeType() const;

        bool DateTimeDirective() const;

        void ParseDateTime(std::vector<std::wstring>* output);

        // Returns locale's and timezone specific time.
        std::wstring GetDateTimeStr(uint64 ticks) const;

        bool ParseEscape(std::vector<std::wstring>* output);

        void WriteCustomUserText(std::vector<std::wstring>* output);

        bool OtherSupportedDirective() const;

        void WriteDirective(std::vector<std::wstring>* output);


        static const std::wstring kDefaultTimeFormat;

        static const wchar_t kDefaultDateType;

        static const std::wstring kTimeFields;
        static const std::wstring kDateFields;
        static const std::wstring kDateTypes;
        static const std::wstring kOtherSupportedFields;

        std::wstring format_;
        std::wstring::iterator fmt_iter_;

        std::vector<const FileInfo*>* input_;
        size_t output_size_;

        wchar_t date_type_;

        bool intrerpret_symbol_as_directive_;
        bool intrerpret_symbol_as_escape_;
    };

} // namespace indexer_common
