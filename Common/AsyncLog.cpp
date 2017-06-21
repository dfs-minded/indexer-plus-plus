// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "AsyncLog.h"

#include <fcntl.h>
#include <mutex>
#include <thread>

#include "Helpers.h"
#include "LogMessagesListener.h"
#include "Macros.h"
#include "WindowsWrapper.h"

namespace indexer_common {

    using namespace std;

    Log& AsyncLog::Instance() {
        static AsyncLog instance;
        return instance;
    }

    AsyncLog::AsyncLog() {
#ifdef WIN32
		CreateDirectory(L"Logs", nullptr);
        log_file_ = _wfopen(L"Logs//IndexerLog.txt", L"w");
        _setmode(_fileno(log_file_), _O_U8TEXT);
#else
        log_file_ = fopen("IndexerLog.txt", "w");
#endif
#if !defined(SINGLE_THREAD)

        NEW_MUTEX

        worker_thread_ = new thread(&AsyncLog::WriteToFile, this);

        helpers::SetThreadName(worker_thread_, "AsyncLog");

#endif
    }

    AsyncLog::~AsyncLog() {
		DELETE_MUTEX
        fclose(log_file_);
    }

    void AsyncLog::Debug(const wstring& message) {
        PLOCK_GUARD
        COMPOSE_MSG(L"DEBUG")
        messages_.push_back(move(msg));
    }

    void AsyncLog::Info(const wstring& message) {
        PLOCK_GUARD
        COMPOSE_MSG(L"INFO");
        messages_.push_back(move(msg));
    }

    void AsyncLog::Warning(const wstring& message) {
        PLOCK_GUARD
        COMPOSE_MSG(L"WARNING");
        messages_.push_back(move(msg));
    }

    void AsyncLog::Error(const wstring& message) {
        PLOCK_GUARD
        COMPOSE_MSG(L"ERROR");
        messages_.push_back(move(msg));
    }

    void AsyncLog::WriteToFile() {
        while (true) {
            this_thread::sleep_for(chrono::seconds(2));

            PLOCK
            swap(messages_, tmp_messages_storage_);
            PUNLOCK

            for (const auto& msg : tmp_messages_storage_) {
                for (const auto& listener : messages_listeners_)
                    listener->OnNewMessage(msg);

                fwprintf(log_file_, L"%s\n", msg.c_str());
            }

            tmp_messages_storage_.clear();
            fflush(log_file_);
        }
    }

} // namespace indexer_common