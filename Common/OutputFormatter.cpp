// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "OutputFormatter.h"

#include <string>

#include "FileInfo.h"
#include "FileInfoHelper.h"
#include "IndexerDateTime.h"

namespace indexer_common {

    using namespace std;

// Thu Aug 23 14:55:02 2001, locale-dependent
    const wstring OutputFormatter::kDefaultTimeFormat = L"c";
    const wchar_t OutputFormatter::kDefaultDateType = L'0';

    const wstring OutputFormatter::kTimeFields = L"@HIMpSXZ";
    const wstring OutputFormatter::kDateFields = L"aAbBcdjmUwWxyY";

    const wstring OutputFormatter::kDateTypes = L"ATC";

    const wstring OutputFormatter::kOtherSupportedFields = L"%hps";


    OutputFormatter::OutputFormatter(vector<const FileInfo*>* const input, wstring fmt /*= std::wstring(L"")*/,
                                     int max_files /*= -1 */)
        : format_(fmt),
          input_(input),
          output_(make_shared<vector<wstring>>()),
          date_type_(kDefaultDateType),
          intrerpret_symbol_as_directive_(false),
          intrerpret_symbol_as_escape_(false) {

        output_size_ = max_files == -1 ? input->size() : min(max_files, input->size());
        setlocale(LC_ALL, ".OCP");
        size_t pos = format_.find(L"%a");
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

        fmt_iter_ = begin(format_);
    }

    std::vector<std::wstring> OutputFormatter::Format() {
        // |output_| could be already moved, call of clear makes its state defined..
        output_.clear();
        output_.resize(output_size_);
        if (DefaultFormat()) {
            WriteDefaultFormat();
            return move(output_);
        }

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
                if (!ParseEscape()) {
                    WriteCustomUserText();
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
                    ParseDateTime();
                    date_type_ = kDefaultDateType;

                } else if (OtherSupportedDirective()) {
                    WriteDirective();

                } else {
                    WriteCustomUserText();
                }

                intrerpret_symbol_as_directive_ = false;
                continue;
            }

            WriteCustomUserText();
        }

        return move(output_);
    }

    bool OutputFormatter::DefaultFormat() {
        return fmt_iter_ == end(format_);
    }

    void OutputFormatter::WriteDefaultFormat() {
        for (size_t i = 0; i < output_.size(); ++i) {
            output_[i] += reinterpret_cast<const wchar_t*>(input_->at(i)->GetName());
        }
    }

    bool OutputFormatter::DateTimeDirective() const {
        wchar_t symbol = *fmt_iter_;

        return (kTimeFields.find(symbol) != string::npos) || (kDateFields.find(symbol) != string::npos);
    }

    bool OutputFormatter::DateTimeType() const {
        return kDateTypes.find(*fmt_iter_) != string::npos;
    }

    void OutputFormatter::ParseDateTime() {
        for (size_t i = 0; i < output_.size(); ++i) {
            const FileInfo* fi = (*input_)[i];
            switch (date_type_) {
                case L'A':
                    output_[i] += GetDateTimeStr(fi->LastAccessTime);
                    break;
                case L'T':
                    output_[i] += GetDateTimeStr(fi->LastWriteTime);
                    break;
                case L'C':
                    output_[i] += GetDateTimeStr(fi->LastAccessTime);
                    break;
            }
        }
    }

    std::wstring OutputFormatter::GetDateTimeStr(uint64 ticks) const {
        wchar_t symbol = *fmt_iter_;
        if (symbol == L'@') {
            return to_wstring(IndexerDateTime::TicksToSeconds(ticks));
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

    void OutputFormatter::WriteCustomUserText() {
        wstring userText;

        if (DateTypeIsSet()) {
            userText += date_type_;
            date_type_ = kDefaultDateType;
        }

        if (intrerpret_symbol_as_escape_) {
            userText += L'\\';
        }

        userText += *fmt_iter_;

        for (size_t i = 0; i < output_.size(); ++i) {
            output_[i] += userText;
        }
    }

    bool OutputFormatter::OtherSupportedDirective() const {
        return kOtherSupportedFields.find(*fmt_iter_) != string::npos;
    }

    void OutputFormatter::WriteDirective() {
        for (size_t i = 0; i < output_.size(); ++i) {
            const FileInfo* fi = (*input_)[i];
            wstring path;

            switch (*fmt_iter_) {
                case L'%':
                    output_[i] += wstring(L"%");
                    break;
                case L'h':
                    path = reinterpret_cast<const wchar_t*>(FileInfoHelper::GetPath(*fi, false).get());
                    output_[i] += path;
                    break;
                case L'p':
                    output_[i] += reinterpret_cast<const wchar_t*>(fi->GetName());
                    break;
                case L's':
                    output_[i] += to_wstring(fi->SizeReal);
                    break;
            }
        }
    }

    bool OutputFormatter::ParseEscape() {
        for (size_t i = 0; i < output_.size(); ++i) {
            const FileInfo* fi = (*input_)[i];
            wstring& currentElem = output_[i];

            switch (*fmt_iter_) {
                case L'a':
                    currentElem += L'\a';
                    break;
                case L'b':
                    currentElem += L'\b';
                    break;
                case L'f':
                    currentElem += L'\f';
                    break;
                case L'n':
                    currentElem += L'\n';
                    break;
                case L'r':
                    currentElem += L'\r';
                    break;
                case L't':
                    currentElem += L'\t';
                    break;
                case L'v':
                    currentElem += L'\v';
                    break;
                case L'\\':
                    currentElem += L'\\';
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