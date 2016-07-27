// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "InteropHelper.h"

#include "WindowsWrapper.h"

#include "IndexerDateTime.h"

using namespace std;
using namespace System;
using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;
// clang-format off

namespace CLIInterop
{
	void InteropHelper::ToUnmanagedString(System::String^ from, std::string& to)
	{
            const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(from)).ToPointer();
            to                = chars;
            Marshal::FreeHGlobal(IntPtr((void*)chars));
        }

		const char16_t* InteropHelper::ToChar16(const System::String^ from) {
            pin_ptr<const wchar_t> convertedValue = PtrToStringChars(from);
			auto constValue = reinterpret_cast<const char16_t*>(convertedValue);
            return constValue;
        }

		const u16string InteropHelper::ToU16string(System::String^ from) {
            return ToChar16(from);
        }


		const wchar_t* InteropHelper::ToWCharT(const System::String ^ from) {
			pin_ptr<const wchar_t> convertedValue = PtrToStringChars(from);
			const wchar_t* constValue = convertedValue;
			return constValue;
		}

		const wstring InteropHelper::ToWstring(System::String ^ from) {
			return ToWCharT(from);
		}


        System::String^ InteropHelper::ToSystemString(wstring from) {
            return ToSystemString(from.c_str());
        }

		System::String^ InteropHelper::ToSystemString(const char16_t* from) {
			return gcnew String(reinterpret_cast<const wchar_t*>(from));
		}

        System::String^ InteropHelper::ToSystemString(const wchar_t* from) {
            return gcnew String(from);
        }


		System::DateTime^ InteropHelper::UnixTimeSecondsToDateTime(uint seconds) {
            Debug::Assert(seconds >= 0);

            // Unix start of the epoch.
            DateTime^ dt = gcnew DateTime(1970, 1, 1, 0, 0, 0, 0, System::DateTimeKind::Utc);

            dt = dt->AddSeconds(seconds).ToLocalTime();
            return dt;
        }

        uint InteropHelper::DateTimeToUnixTimeSeconds(System::DateTime^ dt) {
            _FILETIME ft = IndexerDateTime::TicksToFiletime(dt->ToFileTime());
            return IndexerDateTime::FiletimeToUnixTime(ft);
        }
}