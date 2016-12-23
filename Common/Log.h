// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <list>
#include <string>

#include "CompilerSymb.h"

namespace indexer_common {

    class LogMessagesListener;

#ifdef SINGLE_THREAD_LOG
#define GET_LOGGER logger_ = &OneThreadLog::Instance();
#else
#define GET_LOGGER logger_ = &AsyncLog::Instance();
#endif

#define COMPOSE_MSG(logLevel) \
        std::wstring msg = GetTime() + L" | " + std::wstring(logLevel) + L" | " + GetThreadID() + L" | " + message;

#define METHOD_METADATA indexer_common::helpers::StringToWstring(std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + " ")

#define TIK auto start_time = GetTickCount64();
#define TOK(msg)																							  \
        {                                                                                                     \
            auto elapsed_time = GetTickCount64() - start_time;                                                \
            logger_->Debug(std::wstring(L"PERF | ") + (msg) + std::wstring(L" | Elapsed ") + std::to_wstring(elapsed_time)); \
        }

    class Log {
       public:
        virtual void Debug(const std::wstring& message) = 0;

        virtual void Info(const std::wstring& message) = 0;

        virtual void Warning(const std::wstring& message) = 0;

        virtual void Error(const std::wstring& message) = 0;

        void RegisterMessagesListener(const LogMessagesListener* listener);

        void UnregisterMessagesListener(const LogMessagesListener* listener);

       protected:
        virtual ~Log() = default;

        std::wstring GetTime() const;

        std::wstring GetThreadID() const;

        std::list<const LogMessagesListener*> messages_listeners_;
    };

} // namespace indexer_common