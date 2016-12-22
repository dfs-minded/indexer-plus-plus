// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "Log.h"

#include <sstream>
#include <thread>

#include "HelperCommon.h"
#include "IndexerDateTime.h"

namespace indexer_common {

    using namespace std;

    wstring Log::GetTime() const {
        SYSTEMTIME st = IndexerDateTime::SystemTimeNow();

        char output[30];

#ifdef WIN32  // TODO How to use it on Linux
        sprintf_s(output, 30, "%d-%d-%d.%d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
        sprintf(output, "%d-%d-%d.%d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#endif

        string str(output);

        return HelperCommon::StringToWstring(move(str));
    }

    wstring Log::GetThreadID() const {
        wstringstream ss;
        ss << this_thread::get_id();
        return wstring(L"Thread:") + ss.str();
    }

    void Log::RegisterMessagesListener(const LogMessagesListener* listener) {
        messages_listeners_.push_back(listener);
    }

    void Log::UnregisterMessagesListener(const LogMessagesListener* listener) {
        messages_listeners_.remove(listener);
    }

} // namespace indexer_common