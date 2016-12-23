#pragma once
// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include <functional>
#include <memory>
#include <string>

#include "WindowsWrapper.h"

namespace ntfs_reader {

	namespace helpers {

        std::wstring SerializeRecord(const USN_RECORD& record, char drive);

        // Returns pair of deserialized from the input string |s| USN record (with custom deleter) and
		// corresponding drive letter.
        std::pair<std::unique_ptr<USN_RECORD, std::function<void(USN_RECORD*)>>, char> DeserializeRecord(
            const std::wstring& s);

        std::wstring GetEncodingString(const wchar_t* text);

        std::wstring GetReasonString(DWORD reason);

		/*   private:
			const std::wstring kDelim;

			const std::pair<std::wstring, DWORD> kUSNReasonsPairs[];*/

    } //namespace helpers

} // namespace ntfs_reader