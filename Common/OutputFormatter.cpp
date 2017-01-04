// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "OutputFormatter.h"

#include <string>
#include <algorithm>

#include "FileInfo.h"
#include "FileInfoHelper.h"
#include "IndexerDateTime.h"

namespace indexer_common {

using std::string;
using std::wstring;
using std::vector;

// Thu Aug 23 14:55:02 2001, locale-dependent
const wstring OutputFormatter::kDefaultTimeFormat = L"c";
const wchar_t OutputFormatter::kDefaultDateType = L'0';

const wstring OutputFormatter::kTimeFields = L"@HIMpSXZ";
const wstring OutputFormatter::kDateFields = L"aAbBcdjmUwWxyY";

const wstring OutputFormatter::kDateTypes = L"ATC";

const wstring OutputFormatter::kOtherSupportedFields = L"%hps";


OutputFormatter::OutputFormatter(vector<const FileInfo*>* const input, wstring fmt /*= wstring(L"")*/,
                                 size_t max_files /*= -1 */)
    : format_(fmt),
      input_(input),
      date_type_(kDefaultDateType),
      intrerpret_symbol_as_directive_(false),
      intrerpret_symbol_as_escape_(false) {

    output_size_ = max_files == -1 ? input->size() : std::min(max_files, input->size());

    setlocale(LC_ALL, ".OCP");

    ReplaceSmallLettersInFormatToUppercase();
    }

    void OutputFormatter::ReplaceSmallLettersInFormatToUppercase() {

        auto pos = format_.find(L"%a");
        while (pos != string::npos) {
            format_ = format_.replace(pos + 1, 1, L"A" + kDefaultTimeFormat);
            pos = format_.find(L"%a");
        }

        pos = format_.find(L"%t");
        while (pos != string::npos) {
            format_ = format_.replace(pos + 1, 1, L"T" + kDefaultTimeFormat);
            pos = format_.find(L"%t");
        }

        pos = format_.find(L"%c");
        while (pos != string::npos) {
            format_ = format_.replace(pos + 1, 1, L"C" + kDefaultTimeFormat);
            pos = format_.find(L"%c");
        }
    }

    std::unique_ptr<vector<wstring>> OutputFormatter::Format() {
        fmt_iter_ = begin(format_);

        auto output = std::make_unique<vector<wstring>>(output_size_);

        if (DefaultFormat())
            WriteDefaultFormat(output.get());
        else
            WriteCustomUserFormat(output.get());

        return std::move(output);
    }

    bool OutputFormatter::DefaultFormat() {
        return fmt_iter_ == end(format_);
    }

    void OutputFormatter::WriteDefaultFormat(vector<wstring>* output) {
        for (size_t i = 0; i < output->size(); ++i) {
            (*output)[i] += reinterpret_cast<const wchar_t*>(input_->at(i)->GetName());
        }
    }

    void OutputFormatter::WriteCustomUserFormat(vector<wstring>* output) {
        for (; fmt_iter_ != end(format_); ++fmt_iter_) {
            wchar_t symbol = *fmt_iter_;

            if (symbol == L'%' && !intrerpret_symbol_as_directive_) {
                intrerpret_symbol_as_directive_ = true;
                continue;
            }

            if (symbol == L'\\') {
                intrerpret_symbol_as_escape_ = true;
                continue;
            }

            if (intrerpret_symbol_as_escape_) {
                if (!ParseEscape(output)) {
                    WriteCustomUserText(output);
                }
                intrerpret_symbol_as_escape_ = false;
                continue;
            }

            if (intrerpret_symbol_as_directive_) {
                if (DateTimeType() && !DateTypeIsSet()) {
                    date_type_ = symbol;
                    continue;
                }

                if (DateTimeDirective() && DateTypeIsSet()) {
                    ParseDateTime(output);
                    date_type_ = kDefaultDateType;

                } else if (OtherSupportedDirective()) {
                    WriteDirective(output);

                } else {
                    WriteCustomUserText(output);
                }

                intrerpret_symbol_as_directive_ = false;
                continue;
            }

            WriteCustomUserText(output);
        }
    }


    bool OutputFormatter::DateTimeDirective() const {
        wchar_t symbol = *fmt_iter_;

        return (kTimeFields.find(symbol) != string::npos) || (kDateFields.find(symbol) != string::npos);
    }

    bool OutputFormatter::DateTimeType() const {
        return kDateTypes.find(*fmt_iter_) != string::npos;
    }

    void OutputFormatter::ParseDateTime(vector<wstring>* output) {
        for (size_t i = 0; i < output->size(); ++i) {
            auto fi = (*input_)[i];
            switch (date_type_) {
                case L'A':
                    (*output)[i] += GetDateTimeStr(fi->LastAccessTime);
                    break;
                case L'T':
                    (*output)[i] += GetDateTimeStr(fi->LastWriteTime);
                    break;
                case L'C':
                    (*output)[i] += GetDateTimeStr(fi->LastAccessTime);
                    break;
            }
        }
    }

    wstring OutputFormatter::GetDateTimeStr(uint64 ticks) const {
        wchar_t symbol = *fmt_iter_;
        if (symbol == L'@') {
            return std::to_wstring(IndexerDateTime::TicksToSeconds(ticks));
        }

        FILETIME filetime = IndexerDateTime::TicksToFiletime(ticks);
        FILETIME local_filetime = IndexerDateTime::FiletimeToLocalFiletime(filetime);
        const tm time_info = IndexerDateTime::FiletimeToTM(local_filetime);
        wchar_t buffer[80];

        wstring fmt = L"%" + wstring(1, symbol);
        wcsftime(buffer, 80, fmt.c_str(), &time_info);
        wstring wstr(buffer);
        return wstr;
    }

    void OutputFormatter::WriteCustomUserText(vector<wstring>* output) {
        wstring userText;

        if (DateTypeIsSet()) {
            userText += date_type_;
            date_type_ = kDefaultDateType;
        }

        if (intrerpret_symbol_as_escape_) {
            userText += L'\\';
        }

        userText += *fmt_iter_;

        for (size_t i = 0; i < output->size(); ++i) {
            (*output)[i] += userText;
        }
    }

    bool OutputFormatter::OtherSupportedDirective() const {
        return kOtherSupportedFields.find(*fmt_iter_) != string::npos;
    }

    void OutputFormatter::WriteDirective(vector<wstring>* output) {
        for (size_t i = 0; i < output->size(); ++i) {
            auto fi = (*input_)[i];
            wstring path;

            switch (*fmt_iter_) {
                case L'%':
                    (*output)[i] += wstring(L"%");
                    break;
                case L'h':
                    path = reinterpret_cast<const wchar_t*>(FileInfoHelper::GetPath(*fi, false).get());
                    (*output)[i] += path;
                    break;
                case L'p':
                    (*output)[i] += reinterpret_cast<const wchar_t*>(fi->GetName());
                    break;
                case L's':
                    (*output)[i] += std::to_wstring(fi->SizeReal);
                    break;
            }
        }
    }

    bool OutputFormatter::ParseEscape(vector<wstring>* output) {
        for (size_t i = 0; i < output->size(); ++i) {
            auto fi = (*input_)[i];
            auto& current_elem = (*output)[i];

            switch (*fmt_iter_) {
                case L'a':
                    current_elem += L'\a';
                    break;
                case L'b':
                    current_elem += L'\b';
                    break;
                case L'f':
                    current_elem += L'\f';
                    break;
                case L'n':
                    current_elem += L'\n';
                    break;
                case L'r':
                    current_elem += L'\r';
                    break;
                case L't':
                    current_elem += L'\t';
                    break;
                case L'v':
                    current_elem += L'\v';
                    break;
                case L'\\':
                    current_elem += L'\\';
                    break;
                default:
                    return false;
            }
        }

        return true;
    }

    bool OutputFormatter::DateTypeIsSet() const {
        return date_type_ != kDefaultDateType;
    }

} // namespace indexer_common