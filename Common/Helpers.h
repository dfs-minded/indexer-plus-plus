// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <sstream>
#include <string>
#include <memory>
#include <vector>

#include "WindowsWrapper.h"
#include "macros.h"
#include "typedefs.h"

// TODO get rid of this.
namespace std { class thread; }

namespace indexer_common {

extern const char16_t* Empty16String;

namespace helpers {

	char16_t* CopyU16StringToChar16(const std::u16string& s);

	EXPORT std::wstring StringToWstring(const std::string& str);

	EXPORT std::string WStringToString(const std::wstring& w_str);

	std::wstring U16stringToWstring(const std::u16string& s);

	EXPORT std::u16string WstringToU16string(const std::wstring& s);

	const std::wstring Char16ToWstring(const char16_t* s);

	bool Utf16ToUtf8(const std::u16string& source_utf_16, char* dest_utf_8_buffer, const int buffer_size);

	int Str16Len(const char16_t* s);

	template <typename TChar>
	std::unique_ptr<const TChar[]> CopyToNewWchar(const TChar* source) {
		auto str_len = 0;
		while (*(source + str_len) != '\0') {
			++str_len;
		}

		++str_len;  // One extra for terminating character.

		auto dest = new TChar[str_len];
		memcpy(dest, source, str_len * sizeof(TChar));

		std::unique_ptr<const TChar[]> res(const_cast<const TChar*>(dest));

		return res;
	}

	uint64 PairDwordToInt64(DWORD high, DWORD low);

	uint64 LargeIntegerToInt64(LARGE_INTEGER li);

	std::wstring IntToHex(DWORD x);

	std::u16string ToUpper(std::u16string& s);
	std::wstring ToUpper(std::wstring& s);

	std::wstring GetLastErrorString();

	EXPORT bool DirExist(const std::wstring& path);

	char16_t* GetFilename(const USN_RECORD& record, ushort* name_length);

	template <class T>
	T ParseNumber(const std::wstring& s) {
		std::wistringstream sin(s);
		T res;
		sin >> res;

		if (sin.fail()) {
			std::string str(s.begin(), s.end());
			throw std::invalid_argument(str);
		}
		return res;
	}

	enum class SplitOptions { ONLY_NON_EMPTY, INCLUDE_EMPTY, INCLUDE_SPLITTERS };

	// Splits |s| by |delimeters|.
	// If |option| == ONLY_NON_EMPTY then only non-empty string with no splitters will be included,
	// If |option| == INCLUDE_EMPTY then also empty strings will be included (with no splitters),
	// If |option| == INCLUDE_SPLITTERS then splitters will be included in result, such that
	// consecutive equal splitters will be in the same resulting substring.
	template <typename TStr>
	std::vector<TStr> Split(const TStr& s, const TStr& delimeters,
		SplitOptions options = SplitOptions::ONLY_NON_EMPTY) {
		std::vector<TStr> result;
		TStr tmp;

		for (size_t i = 0; i < s.size();) {

			if (delimeters.find(s[i]) == -1) {
				tmp += s[i++];
				continue;
			}

			if (!tmp.empty() || options == SplitOptions::INCLUDE_EMPTY) {
				result.push_back(move(tmp));
				tmp.clear();
			}

			if (options == SplitOptions::INCLUDE_SPLITTERS) {
				auto splitter = s[i];

				for (; i < s.size() && s[i] == splitter; ++i)
					tmp += splitter;

				result.push_back(move(tmp));
				tmp.clear();
			}
			else {
				++i;
			}
		}

		if (!tmp.empty() || options == SplitOptions::INCLUDE_EMPTY) {
			result.push_back(move(tmp));
		}

		return result;
	}

	void SetThreadName(std::thread* thread, const char* thread_name);

	bool IsAsciiString(const wchar_t* s);

		// Returns "C:", "D:" etc. A result is on heap, the caller is responsible for deleting it.
	char16_t* GetDriveName(char16_t drive_letter);

	uint GetNumberOfProcessors();

	int SizeFromBytesToKiloBytes(uint64 size_in_bytes);

	void WriteToOutput(const std::string& s);
	void WriteToOutput(const std::wstring& s);

} // namespace helpers
} // namespace indexer_common
