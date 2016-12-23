// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "InteropHelper.h"

#include <vcclr.h>

#include "IndexerDateTime.h"

using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;
// clang-format off

namespace CLIInterop
{
	namespace Helper 
	{
		void ToUnmanagedString(System::String^ from, std::string& to)
		{
			const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(from)).ToPointer();
			to = chars;
			Marshal::FreeHGlobal(System::IntPtr((void*)chars));
		}

		const char16_t* ToChar16(const System::String^ from) {
			pin_ptr<const wchar_t> convertedValue = PtrToStringChars(from);
			auto constValue = reinterpret_cast<const char16_t*>(convertedValue);
			return constValue;
		}

		const std::u16string ToU16string(System::String^ from) {
			return ToChar16(from);
		}


		const wchar_t* ToWCharT(const System::String^ from) {
			pin_ptr<const wchar_t> convertedValue = PtrToStringChars(from);
			const wchar_t* constValue = convertedValue;
			return constValue;
		}

		const std::wstring ToWstring(System::String^ from) {
			return ToWCharT(from);
		}


		System::String^ ToSystemString(std::wstring from) {
			return ToSystemString(from.c_str());
		}

		System::String^ ToSystemString(const char16_t* from) {
			return gcnew System::String(reinterpret_cast<const wchar_t*>(from));
		}

		System::String^ ToSystemString(const wchar_t* from) {
			return gcnew System::String(from);
		}


		System::DateTime^ UnixTimeSecondsToDateTime(indexer_common::uint seconds) {
			Debug::Assert(seconds >= 0);

			// Unix start of the epoch.
			System::DateTime^ dt = gcnew System::DateTime(1970, 1, 1, 0, 0, 0, 0, System::DateTimeKind::Utc);

			dt = dt->AddSeconds(seconds).ToLocalTime();
			return dt;
		}

		indexer_common::uint DateTimeToUnixTimeSeconds(System::DateTime^ dt) {
			_FILETIME ft = indexer_common::IndexerDateTime::TicksToFiletime(dt->ToFileTime());
			return indexer_common::IndexerDateTime::FiletimeToUnixTime(ft);
		}

	} // namespace Helper

} // namespace CLIInterop