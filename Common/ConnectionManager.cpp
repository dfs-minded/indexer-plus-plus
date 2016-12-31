// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "ConnectionManager.h"

#include <string>
#include <thread>
#include <vector>

#include "AsyncLog.h"
#include "Helpers.h"
#include "Log.h"
#include "WindowsWrapper.h"

namespace indexer_common {

    using namespace std;

    const wchar_t ConnectionManager::kDefaultPipeName[] = L"Indexer++NamedPipe";

    const wstring ConnectionManager::kLastMessageIndicator = L"|";

#ifdef WIN32

    ConnectionManager::ConnectionManager() {
        GET_LOGGER
    }

    void ConnectionManager::CreateServer(pIQueryProcessor queryProcessor) {
        query_processor_ = queryProcessor;
        thread worker(&ConnectionManager::ServerWorker, *this);
        helpers::SetThreadName(&worker, "NamedPipeServerWorker ");
        worker.detach();
    }

    void ConnectionManager::ServerWorker() {

        while (true) {
            HANDLE pipe = CreatePipe(kDefaultPipeName);
            if (pipe == INVALID_HANDLE_VALUE) {
                logger_->Error(L"Invalid pipe " + GetLastError());
                return;
            }

            bool connected = ConnectNamedPipe(pipe, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
            if (!connected) {
                logger_->Error(METHOD_METADATA + L"Cannot connect to the named pipe.");
                CloseHandle(pipe);
            }

            thread reply_runner(&ConnectionManager::Reply, *this, pipe);
            helpers::SetThreadName(&reply_runner, "ReplyFromNamedPipeServer ");
            reply_runner.detach();
        }
    }

    void ConnectionManager::Reply(HANDLE pipe) {
        wstring message;
        TCHAR buffer[kBufSize + 1];
        ReadMessageFromPipe(pipe, buffer, &message);

        if (message.empty()) return;

        wstring data_pipename, query, format;
        int max_files;
        ParseData(message, &query, &format, &max_files);

        auto result = query_processor_->Process(query, format, max_files);

        for (const wstring& s : *result) {
            if (!WriteMessageToPipe(pipe, s)) {
                logger_->Error(METHOD_METADATA + L"Cannot write message to named pipe.");
                break;
            }
        }

        WriteMessageToPipe(pipe, kLastMessageIndicator);  // Last message.
        logger_->Info(METHOD_METADATA + L"Search result is sent to a client, number of files = " +
                      to_wstring(result->size()));

        CloseHandle(pipe);
    }


    const wstring delimeter = L";";
    const wchar_t* kConnectionError =
        L"Error connecting to Indexer++ process. Maybe the process was not started or cmd client was not run as "
        L"administrator.";

    bool ConnectionManager::SendQuery(const wstring& query, const wstring& format, int max_files, vector<wstring>* result) {
        TCHAR buffer[kBufSize + 1];
        HANDLE pipe = OpenPipe(kDefaultPipeName);

        wstring message = query + delimeter + format + delimeter + to_wstring(max_files);

        bool ok = WriteMessageToPipe(pipe, message);
        if (!ok) {
            result->push_back(kConnectionError);
            return false;
        }

        wstring tmp;
        while (true) {
            ok = ReadMessageFromPipe(pipe, buffer, &tmp);
            if (!ok) {
                // Probably the server is not ready yet.
                Sleep(100);
                continue;
            }

            if (tmp == kLastMessageIndicator) break;

            result->push_back(move(tmp));
        }

        return true;
    }

    HANDLE ConnectionManager::CreatePipe(const wstring& pipename) {
        return CreateNamedPipe((L"\\\\.\\pipe\\" + pipename).c_str(),  // pipe name
                               PIPE_ACCESS_DUPLEX,                     // read/write access
                               PIPE_TYPE_MESSAGE |                     // message type pipe
                                   PIPE_READMODE_MESSAGE |             // message-read mode
                                   PIPE_WAIT,                          // blocking mode
                               PIPE_UNLIMITED_INSTANCES,               // max. instances
                               kBufSize,                               // output buffer size
                               kBufSize,                               // input buffer size
                               0,                                      // client time-out
                               NULL);                                  // default security attribute
    }

    HANDLE ConnectionManager::OpenPipe(const wstring& pipename) {
        HANDLE pipe = CreateFile((L"\\\\.\\pipe\\" + pipename).c_str(),  // pipe name
                                 GENERIC_READ | GENERIC_WRITE,           // read/write access
                                 0,                                      // no sharing
                                 NULL,                                   // default security attributes
                                 OPEN_EXISTING,                          // opens existing pipe
                                 0,                                      // default attributes
                                 NULL);                                  // no template file
        DWORD mode = PIPE_READMODE_MESSAGE;
        BOOL ok = SetNamedPipeHandleState(pipe,   // pipe handle
                                          &mode,  // new pipe mode
                                          NULL,   // don't set maximum bytes
                                          NULL);  // don't set maximum time
        if (!ok) {
            logger_->Error(L"SetNamedPipeHandleState failed, GetLastError = "+ GetLastError());
            CloseHandle(pipe);
            return INVALID_HANDLE_VALUE;
        }

        return pipe;
    }

    void ConnectionManager::ParseData(const wstring& data, wstring* query, wstring* format, int* max_files) {

        auto parts = helpers::Split(data, delimeter, helpers::SplitOptions::INCLUDE_EMPTY);
        *query = parts[0];
        *format = parts[1];
        *max_files = helpers::ParseNumber<int>(parts[2]);
    }

    bool ConnectionManager::WriteMessageToPipe(HANDLE pipe, const wstring& message) {
        DWORD bytes_written;
        return WriteFile(pipe, message.c_str(), 2 * message.size(), &bytes_written, NULL /* not overlapped */) != 0;
    }

    bool ConnectionManager::ReadMessageFromPipe(HANDLE pipe, TCHAR* buffer, wstring* result) {
        DWORD bytes_read;
        bool ok = ReadFile(pipe, buffer, kBufSize * sizeof(TCHAR), &bytes_read, NULL /*not overlapped */) != 0;
        *result = wstring(buffer, bytes_read / 2);
        return ok;
    }

#else  // For Linux compilation.
    void ConnectionManager::CreateServer(pIQueryProcessor queryProcessor) {
    }
    void ConnectionManager::ServerWorker() {
    }
    void ConnectionManager::SendQuery(const wstring& query, wstring format, vector<wstring>& result) {
    }
    HANDLE ConnectionManager::CreatePipe(const wstring& pipename) {
        return 0;
    }
    HANDLE ConnectionManager::OpenPipe(const wstring& pipename) {
        return 0;
    }
    void ConnectionManager::ParseData(const wstring& data, wstring& pipename, wstring& query, wstring& format) {
    }
    bool ConnectionManager::WriteMessageToPipe(HANDLE pipe, const wstring& message) {
        return true;
    }
    bool ConnectionManager::ReadMessageFromPipe(HANDLE pipe, wstring& result) {
        return true;
    }
    void ConnectionManager::Reply(const wstring& query, const wstring& format, const wstring& data_pipename) {
    }
#endif
} // namespace indexer_common