// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "Log.h"

#include <sstream>
#include <thread>

#include "Helpers.h"
#include "IndexerDateTime.h"

namespace indexer_common {

	std::wstring Log::GetTime() const {
        SYSTEMTIME st = IndexerDateTime::SystemTimeNow();

        char output[30];

#ifdef WIN32  // TODO How to use it on Linux
        sprintf_s(output, 30, "%d-%d-%d.%d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
        sprintf(output, "%d-%d-%d.%d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#endif

        std::string str(output);

		return helpers::StringToWstring(move(str));
    }

    std::wstring Log::GetThreadID() const {
		std::wstringstream ss;
		ss << std::this_thread::get_id();
		return std::wstring(L"Thread:") + ss.str();
    }

    void Log::RegisterMessagesListener(const LogMessagesListener* listener) {
        messages_listeners_.push_back(listener);
    }

    void Log::UnregisterMessagesListener(const LogMessagesListener* listener) {
        messages_listeners_.remove(listener);
    }

} // namespace indexer_common