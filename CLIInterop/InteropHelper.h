// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <string>

#include "typedefs.h"
// clang-format off

namespace CLIInterop
{
	namespace Helper
	{
		void ToUnmanagedString(System::String^ from, std::string& to);


		const char16_t* ToChar16(const System::String^ from);

		const std::u16string ToU16string(System::String^ from);


		const wchar_t* ToWCharT(const System::String^ from);

		const std::wstring ToWstring(System::String^ from);


		System::String^ ToSystemString(const std::wstring from);

		System::String^ ToSystemString(const char16_t* from);

		System::String^ ToSystemString(const wchar_t* from);

		System::DateTime^ UnixTimeSecondsToDateTime(indexer_common::uint seconds);

		indexer_common::uint DateTimeToUnixTimeSeconds(System::DateTime^ dt);

	} // namespace Helper

} // namespace CLIInterop