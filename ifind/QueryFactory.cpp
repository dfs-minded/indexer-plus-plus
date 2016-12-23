// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "QueryFactory.h"

#include "Helpers.h"

#include <stdlib.h>
#include <iostream>

#include "IndexerDateTime.h"

namespace ifind {

	using std::string;
	using std::wstring;

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

	int QueryFactory::TryParseSize(const std::wstring& text) {
		long multiplier = 1;  // if nothing specified, default is Kilobytes
		int number_length = text.size();

		// try to find unit name.
		if (!isdigit(text.back())) {
			if (multipliers.find(text.back()) == multipliers.end())
				throw std::invalid_argument(static_cast<char>(text.back()) + std::string(" is not valid unit for size"));
			multiplier = multipliers[text.back()];
			--number_length;
		}

		auto size = indexer_common::helpers::ParseNumber<long>(text.substr(0, number_length));
		return size * multiplier;
	}

	indexer_common::uSearchQuery QueryFactory::ParseInput(const std::vector<wstring>& args, wstring* format, wstring* outputPath) {

		using namespace indexer_common;

		wstring searchText = L"";
		wstring searchDirPath = L"";
		bool matchCase = false;
		int sizeFrom = 0;
		int sizeTo = INT_MAX;
		uint cTimeFrom = 0;
		uint cTimeTo = UINT_MAX;
		uint aTimeFrom = 0;
		uint aTimeTo = UINT_MAX;
		uint mTimeFrom = 0;
		uint mTimeTo = UINT_MAX;
		bool excludeHidden = false;
		bool excludeFolders = false;
		bool excludeFiles = false;

		size_t argNumber = 0;
		wstring path = args[0];
		bool pathSpecified = path[0] != '-';

		if (pathSpecified) {

			if (path == L".")  // If search current folder.
			{
				searchDirPath = CurrentDir();

			}
			else if (path[0] == '/') {  // Search the whole system.

				// Leave searchDirPath empty.

			}
			else if (path[0] == '.') {  // If current folder sub folder e.g. "./MyDir/".

				searchDirPath = CurrentDir() + path.substr(1);

			}
			else {  // Full path specified.

				searchDirPath = path;
			}
			
			if (pathSpecified && searchDirPath != L"" && !indexer_common::helpers::DirExist(searchDirPath)) {
				std::wcout << L"'" << searchDirPath << L"': No such directory" << std::endl;
			}

			++argNumber;
		}

		for (; argNumber < args.size(); ++argNumber) {

			wstring arg = args[argNumber];

			if (arg == L"-name" || arg == L"-iname") {

				if (searchText != L"") throw std::invalid_argument("file name filter already specified");

				matchCase = arg == L"-name";
				++argNumber;
				searchText = args[argNumber];

			}
			else if (arg == L"-cmin") {
				wstring timeText = args[++argNumber];
				try {
					SetTime(timeText, &cTimeFrom, &cTimeTo, indexer_common::DateTimeEnum::Minutes);
				}
				catch (const std::invalid_argument& ia) {
					throw std::invalid_argument(string(ia.what()) + "to -cmin");
				}

				if (cTimeFrom > cTimeTo) {
					throw std::range_error("-cmin invalid time range");
				}

			}
			else if (arg == L"-ctime") {

				wstring timeText = args[++argNumber];

				try {
					SetTime(timeText, &cTimeFrom, &cTimeTo, indexer_common::DateTimeEnum::Days);
				}
				catch (const std::invalid_argument& ia) {
					throw std::invalid_argument(string(ia.what()) + "to -ctime");
				}

				if (cTimeFrom > cTimeTo) {
					throw std::range_error("-ctime invalid time range");
				}

			}
			else if (arg == L"-amin") {

				wstring timeText = args[++argNumber];

				try {
					SetTime(timeText, &aTimeFrom, &aTimeTo, indexer_common::DateTimeEnum::Minutes);
				}
				catch (const std::invalid_argument& ia) {
					throw std::invalid_argument(string(ia.what()) + "to -amin");
				}

				if (aTimeFrom > aTimeTo) {
					throw std::range_error("-amin invalid time range");
				}

			}
			else if (arg == L"-atime") {

				wstring timeText = args[++argNumber];

				try {
					SetTime(timeText, &aTimeFrom, &aTimeTo, indexer_common::DateTimeEnum::Days);
				}
				catch (const std::invalid_argument& ia) {
					throw std::invalid_argument(string(ia.what()) + "to -atime");
				}

				if (aTimeFrom > aTimeTo) {
					throw std::range_error("-atime invalid time range");
				}

			}
			else if (arg == L"-mmin") {

				wstring timeText = args[++argNumber];

				try {
					SetTime(timeText, &mTimeFrom, &mTimeTo, indexer_common::DateTimeEnum::Minutes);
				}
				catch (const std::invalid_argument& ia) {
					throw std::invalid_argument(string(ia.what()) + "to -mmin");
				}

				if (mTimeFrom > mTimeTo) {
					throw std::range_error("-mmin invalid time range");
				}

			}
			else if (arg == L"-mtime") {

				wstring timeText = args[++argNumber];

				try {
					SetTime(timeText, &mTimeFrom, &mTimeTo, indexer_common::DateTimeEnum::Days);
				}
				catch (const std::invalid_argument& ia) {
					throw std::invalid_argument(string(ia.what()) + "to -mtime");
				}

				if (mTimeFrom > mTimeTo) {
					throw std::range_error("-mtime invalid time range");
				}

			}
			else if (arg == L"-size") {

				++argNumber;
				wstring sizeText = args[argNumber];
				SetSize(sizeText, &sizeFrom, &sizeTo);

			}
			else if (arg == L"-type") {

				++argNumber;
				wstring type = args[argNumber];
				SetType(type, &excludeFiles, &excludeFolders);

			}
			else if (arg == L"-fprint") {

				++argNumber;
				*outputPath = args[argNumber];

			}
			else if (arg == L"-fprintf") {

				++argNumber;
				*outputPath = args[argNumber];
				++argNumber;
				*format = args[argNumber];
			}
		}

		auto res = std::make_unique<indexer_common::SearchQuery>(indexer_common::helpers::WstringToU16string(searchText),
			indexer_common::helpers::WstringToU16string(searchDirPath), matchCase, false, sizeFrom,
			sizeTo, excludeHidden, excludeFolders, excludeFiles, cTimeFrom, cTimeTo,
			aTimeFrom, aTimeTo, mTimeFrom, mTimeTo);
		return res;
	}

	// Must be implemented like conjunction of all size restrictions.
	void QueryFactory::SetSize(const wstring& sizeText, int* sizeFrom, int* sizeTo) {
		if (sizeText[0] == '+') {
			int size = TryParseSize(sizeText.substr(1));
			*sizeFrom = max(*sizeFrom, size);
		}
		else if (sizeText[0] == '-') {
			int size = TryParseSize(sizeText.substr(1));
			*sizeTo = min(*sizeTo, size);
		}
		else {  // exact (precise) size
			int size = TryParseSize(sizeText);
			*sizeFrom = max(*sizeFrom, size);
			*sizeTo = min(*sizeTo, size);
		}

		if (*sizeFrom > *sizeTo) {
			throw std::range_error("-size invalid size range ['" + std::to_string(*sizeFrom) + "', '" + std::to_string(*sizeTo) + "']");
		}
	}

	void QueryFactory::SetType(const wstring& type, bool* excludeFiles, bool* excludeFolders) {
		if (type == L"d") {
			*excludeFiles = true;
		}
		else if (type == L"f") {
			*excludeFolders = true;
		}
		else {
			throw std::invalid_argument("invalid argument '" + indexer_common::helpers::WStringToString(type) + "' to -type");
		}
	}

	// Each time option can narrow the output time filter range [|timeFrom|; |timeTo|] from [0; MaxTicks] to empty.
	void QueryFactory::SetTime(const wstring& timeText, indexer_common::uint* timeFrom, indexer_common::uint* timeTo, indexer_common::DateTimeEnum timeType) {
		auto* dt_now = indexer_common::IndexerDateTime::Now();

		if (timeText[0] == '+') {
			double time = indexer_common::helpers::ParseNumber<double>(timeText.substr(1));
			if (time == 0) {
				time = 1;  // zero means within the past 1 day, according to documentation
			}
			dt_now->Add(-1 * time, timeType);
			*timeTo = min(*timeTo, dt_now->UnixSeconds());

		}
		else if (timeText[0] == '-') {
			double time = indexer_common::helpers::ParseNumber<double>(timeText.substr(1));
			dt_now->Add(-1 * time, timeType);
			*timeFrom = max(*timeFrom, dt_now->UnixSeconds());

		}
		else {  // from |time - 1| to |time|
			double time = indexer_common::helpers::ParseNumber<double>(timeText);

			dt_now->Add(-1 * time, timeType);
			*timeTo = min(*timeTo, dt_now->UnixSeconds());

			dt_now->Add(-1, timeType);
			*timeFrom = max(*timeFrom, dt_now->UnixSeconds());
		}
	}

} // namespace ifind