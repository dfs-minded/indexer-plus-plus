// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "UserArgsParser.h"

#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#include "Helpers.h"
#include "IndexerDateTime.h"
#include "SearchQueryBuilder.h"

namespace ifind {

	using std::string;
	using std::wstring;
    using std::to_string;

    namespace {

	const std::map<wchar_t, int> kSizeMultipliers{ 
		{ L'c', 1 },
		{ L'B', 1 },
		{ L'k', 1024 },
		{ L'M', 1024 * 1024 },
		{ L'G', 1024 * 1024 * 1024 } };

	const std::map<wstring, wstring> kTwoLetterToOneSizeMultipliersMap{
		{ wstring(L"KB"), wstring(L"k") },
		{ wstring(L"MB"), wstring(L"M") },
		{ wstring(L"GB"), wstring(L"G") } };


    // Returnts the current execution directory.
    wstring CurrentDir() {

        wchar_t* buffer;

        // Get the current working directory:
        if ((buffer = _wgetcwd(nullptr, 0)) == nullptr) {
            throw "Cannot retrieve current dir. Please, specify the search path explicitly.";
        }

        wstring res(buffer);
        free(buffer);
        return res;
    }


	void NormalizeSizeSpecifierToOneLetter(wstring* size_text) {
		// if the last but one is not a letter - nothing to normalize
		auto last_but_one_indx = size_text->length() - 2;
		if (isdigit(size_text->at(last_but_one_indx))) return;

		for(auto& size_specif : kTwoLetterToOneSizeMultipliersMap) {
			auto fount_pos = size_text->find(size_specif.first, last_but_one_indx);
			if(fount_pos != string::npos) {
				(*size_text) = size_text->replace(fount_pos, size_specif.first.length(), size_specif.second);
				return;
			}
		}
	}

	int TryParseSize(wstring&& text) {
		long multiplier = 1;  // If nothing specified, default is kilobytes.
		int number_len = text.size();

		if (!isdigit(text.back())) {
			
			NormalizeSizeSpecifierToOneLetter(&text);

			// Try to find multiplier value by name.
			auto multiplier_val = kSizeMultipliers.find(text.back());

			if (multiplier_val == kSizeMultipliers.end())
				throw std::invalid_argument(static_cast<char>(text.back()) + string(" is not valid unit for size"));

			multiplier = multiplier_val->second;
			--number_len;
		}

		auto size = indexer_common::helpers::ParseNumber<long>(std::move(text.substr(0, number_len)));
		return size * multiplier;
	}


	// Must be implemented like conjunction of all size restrictions.
	// Making a copy of the original user input arg |size_text|, because we may need to modify it.
	void SetSize(wstring size_text, int* size_from, int* size_to) {
		if (size_text.front() == '+') {
			auto size = TryParseSize(std::move(size_text.substr(1)));
			*size_from = std::max(*size_from, size);
		}
		else if (size_text.front() == '-') {
			auto size = TryParseSize(std::move(size_text.substr(1)));
			*size_to = std::min(*size_to, size);
		}
		else {  // exact (precise) size
			auto size = TryParseSize(std::move(size_text));
			*size_from = std::max(*size_from, size);
			*size_to = std::min(*size_to, size);
		}

		if (*size_from > *size_to) {
			throw std::range_error("-size invalid size range ['" + to_string(*size_from) + "', '" +
				to_string(*size_to) + "']");
		}
	}


	void SetType(const wstring& type, bool* exclude_files, bool* exclude_folders) {
		if (type == L"d") {
			*exclude_files = true;
		}
		else if (type == L"f") {
			*exclude_folders = true;
		}
		else {
			throw std::invalid_argument("invalid argument '" + *indexer_common::helpers::WStringToString(type) +
				"' to -type");
		}
	}


	// Each time option can narrow the output time filter range [|timeFrom|; |timeTo|] from [0; MaxTicks] to empty.
	void SetTime(const wstring& time_text, indexer_common::uint* time_from,
		indexer_common::uint* time_to, indexer_common::DateTimeEnum timeType) {

		auto* dt_now = indexer_common::IndexerDateTime::Now();

		if (time_text[0] == '+') {
			double time = indexer_common::helpers::ParseNumber<double>(time_text.substr(1));
			if (time == 0) {
				time = 1;  // Zero means within the past 1 day, according to the documentation.
			}
			dt_now->Add(-1 * time, timeType);
			*time_to = std::min(*time_to, dt_now->UnixSeconds());

		}
		else if (time_text[0] == '-') {
			double time = indexer_common::helpers::ParseNumber<double>(time_text.substr(1));
			dt_now->Add(-1 * time, timeType);
			*time_from = std::max(*time_from, dt_now->UnixSeconds());

		}
		else {  // From |time - 1| to |time|.
			double time = indexer_common::helpers::ParseNumber<double>(time_text);

			dt_now->Add(-1 * time, timeType);
			*time_to = std::min(*time_to, dt_now->UnixSeconds());

			dt_now->Add(-1, timeType);
			*time_from = std::max(*time_from, dt_now->UnixSeconds());
		}
	}


    } // namespace


    indexer_common::uSearchQuery ComposeQueryFromUserInput(
		const std::vector<wstring>& args, wstring* format, wstring* output_path) {

        using namespace indexer_common;

        wstring search_text = L"";
        wstring search_dir_path = L"";
        bool match_case = false;
        int size_from = 0;
        int size_to = INT_MAX;
        uint c_time_from = 0;
        uint c_time_to = UINT_MAX;
        uint a_time_from = 0;
        uint a_time_to = UINT_MAX;
        uint m_time_from = 0;
        uint m_time_to = UINT_MAX;
        bool exclude_hidden = false;
        bool exclude_folders = false;
        bool exclude_files = false;

        size_t args_num = 0;
        wstring path = args[0];
        bool path_specified = path[0] != '-';

        if (path_specified) {

            if (path == L".") {  // If search current folder.
                search_dir_path = CurrentDir();
            } else if (path[0] == '/') {  // Search the whole system.
                // Leave searchDirPath empty.
            } else if (path[0] == '.') {  // If current folder sub folder e.g. "./MyDir/".
                search_dir_path = CurrentDir() + path.substr(1);
            } else {  // Full path specified.
                search_dir_path = path;
            }

            if (path_specified && search_dir_path != L"" && !indexer_common::helpers::DirExist(search_dir_path)) {
                std::wcout << L"'" << search_dir_path << L"': No such directory" << std::endl;
            }

            ++args_num;
        }

        for (; args_num < args.size(); ++args_num) {

            wstring arg = args[args_num];

            if (arg == L"-name" || arg == L"-iname") {

                if (search_text != L"") throw std::invalid_argument("file name filter already specified");

                match_case = arg == L"-name";
                ++args_num;
                search_text = args[args_num];

            } else if (arg == L"-cmin") {
                wstring timeText = args[++args_num];
                try {
                    SetTime(timeText, &c_time_from, &c_time_to, indexer_common::DateTimeEnum::Minutes);
                } catch (const std::invalid_argument& ia) {
                    throw std::invalid_argument(string(ia.what()) + "to -cmin");
                }

                if (c_time_from > c_time_to) {
                    throw std::range_error("-cmin invalid time range");
                }

            } else if (arg == L"-ctime") {

                wstring timeText = args[++args_num];

                try {
                    SetTime(timeText, &c_time_from, &c_time_to, indexer_common::DateTimeEnum::Days);
                } catch (const std::invalid_argument& ia) {
                    throw std::invalid_argument(string(ia.what()) + "to -ctime");
                }

                if (c_time_from > c_time_to) {
                    throw std::range_error("-ctime invalid time range");
                }

            } else if (arg == L"-amin") {

                wstring timeText = args[++args_num];

                try {
                    SetTime(timeText, &a_time_from, &a_time_to, indexer_common::DateTimeEnum::Minutes);
                } catch (const std::invalid_argument& ia) {
                    throw std::invalid_argument(string(ia.what()) + "to -amin");
                }

                if (a_time_from > a_time_to) {
                    throw std::range_error("-amin invalid time range");
                }

            } else if (arg == L"-atime") {

                wstring timeText = args[++args_num];

                try {
                    SetTime(timeText, &a_time_from, &a_time_to, indexer_common::DateTimeEnum::Days);
                } catch (const std::invalid_argument& ia) {
                    throw std::invalid_argument(string(ia.what()) + "to -atime");
                }

                if (a_time_from > a_time_to) {
                    throw std::range_error("-atime invalid time range");
                }

            } else if (arg == L"-mmin") {

                wstring timeText = args[++args_num];

                try {
                    SetTime(timeText, &m_time_from, &m_time_to, indexer_common::DateTimeEnum::Minutes);
                } catch (const std::invalid_argument& ia) {
                    throw std::invalid_argument(string(ia.what()) + "to -mmin");
                }

                if (m_time_from > m_time_to) {
                    throw std::range_error("-mmin invalid time range");
                }

            } else if (arg == L"-mtime") {

                wstring timeText = args[++args_num];

                try {
                    SetTime(timeText, &m_time_from, &m_time_to, indexer_common::DateTimeEnum::Days);
                } catch (const std::invalid_argument& ia) {
                    throw std::invalid_argument(string(ia.what()) + "to -mtime");
                }

                if (m_time_from > m_time_to) {
                    throw std::range_error("-mtime invalid time range");
                }

            } else if (arg == L"-size") {

                ++args_num;
                wstring size_text = args[args_num];
                SetSize(size_text, &size_from, &size_to);

            } else if (arg == L"-type") {

                ++args_num;
                wstring type = args[args_num];
                SetType(type, &exclude_files, &exclude_folders);

            } else if (arg == L"-fprint") {

                ++args_num;
                *output_path = args[args_num];

            } else if (arg == L"-fprintf") {

                ++args_num;
                *output_path = args[args_num];
                ++args_num;
                *format = args[args_num];
            }
        }


		indexer_common::SearchQueryBuilder builder;

		builder.SetSearchText(*indexer_common::helpers::WstringToU16string(search_text))
			.SetSearchDirPath(*indexer_common::helpers::WstringToU16string(search_dir_path))
			.SetSizeFrom(size_from).SetSizeTo(size_to)
			.SetCreationTimeFrom(c_time_from).SetCreationTimeTo(c_time_to)
			.SetLastAccessTimeFrom(a_time_from).SetLastAccessTimeTo(a_time_to)
			.SetModificationTimeFrom(m_time_from).SetModificationTimeTo(m_time_to);

		if (match_case) builder.SetMatchCase();
		if (exclude_hidden) builder.SetExcludeHiddenAndSystem();
		if (exclude_folders) builder.SetExcludeFolders();
		if (exclude_files) builder.SetExcludeFiles();

		return builder.Build();
    }

} // namespace ifind