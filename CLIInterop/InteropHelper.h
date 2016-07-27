// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <vcclr.h>
#include <string>
#include "WindowsWrapper.h"

#include "typedefs.h"
// clang-format off

namespace CLIInterop
{
	ref class InteropHelper
	{
	public:
		static void ToUnmanagedString(System::String^ from, std::string& to);


		static const char16_t* ToChar16(const System::String^ from);

		static const std::u16string ToU16string(System::String^ from);


		static const wchar_t* ToWCharT(const System::String^ from);

		static const std::wstring ToWstring(System::String^ from);


		static System::String^ ToSystemString(const std::wstring from);

		static System::String^ ToSystemString(const char16_t* from);

		static System::String^ ToSystemString(const wchar_t* from);

		static System::DateTime^ UnixTimeSecondsToDateTime(uint seconds);

		static uint DateTimeToUnixTimeSeconds(System::DateTime^ dt);
	};
}