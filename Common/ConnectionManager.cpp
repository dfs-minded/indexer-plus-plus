// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "ConnectionManager.h"

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "AsyncLog.h"
#include "HelperCommon.h"
#include "Log.h"
#include "WindowsWrapper.h"

using namespace std;

const wchar_t ConnectionManager::kDefaultPipeName[] = L"Indexer++NamedPipe";

const wstring ConnectionManager::kLastMessageIndicator = L"|";

#ifdef WIN32

ConnectionManager::ConnectionManager() {
    GET_LOGGER
}

void ConnectionManager::CreateServer(pIQueryProcessor queryProcessor) {
    query_processor_ = queryProcessor;

    pipe_ = CreatePipe(kDefaultPipeName);
    if (pipe_ == INVALID_HANDLE_VALUE) {
        wcout << L"Invalid pipe " << GetLastError() << endl;
        return;
    }

    thread worker(&ConnectionManager::ServerWorker, *this);
    HelperCommon::SetThreadName(&worker, "NamedPipeServerWorker ");
    worker.detach();
}

void ConnectionManager::ServerWorker() {

    while (true) {

        bool connected = ConnectNamedPipe(pipe_, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (!connected) {
            logger_->Error(METHOD_METADATA + L"Cannot connect to the named pipe.");
        }

        wstring message;

        ReadMessageFromPipe(pipe_, message);

        if (message.empty()) continue;

        wstring data_pipename, query, format;
        int max_files;

        ParseData(message, &data_pipename, &query, &format, &max_files);

        thread reply_runner(&ConnectionManager::Reply, *this, query, format, data_pipename, max_files);
        HelperCommon::SetThreadName(&reply_runner, "ReplyFromNamedPipeServer ");
        reply_runner.detach();
    }

    CloseHandle(pipe_);
}


void ConnectionManager::Reply(const wstring& query, const wstring& format, const wstring& data_pipename,
                              int max_files) {

    vector<wstring> result = query_processor_->Process(query, format, max_files);

    HANDLE data_pipe = OpenPipe(data_pipename);

    for (const wstring& s : result) {

        if (!WriteMessageToPipe(data_pipe, s)) {
            logger_->Error(METHOD_METADATA + L"Cannot write message to named pipe.");
            break;
        }
    }

    WriteMessageToPipe(data_pipe, kLastMessageIndicator);  // Last message.
    CloseHandle(data_pipe);
}


const wstring delimeter = L";";
const wchar_t* kConnectionError = L"Error connecting to Indexer++ process. May be the process was not started or cmd client was not run as administrator.";

bool ConnectionManager::SendQuery(const wstring& query, const wstring& format, int max_files, vector<wstring>* result) {

    HANDLE main_pipe = OpenPipe(kDefaultPipeName);
    wstring data_pipe_name = L"indexer" + to_wstring(GetCurrentProcessId());
    HANDLE data_pipe = CreatePipe(data_pipe_name);

    wstring message = data_pipe_name + delimeter + query + delimeter + format + delimeter + to_wstring(max_files);

    bool ok = WriteMessageToPipe(main_pipe, message);
    if (!ok) {
        result->push_back(kConnectionError);
        return false;
    }

    bool connected = ConnectNamedPipe(data_pipe, nullptr);
    if (!connected) {
        result->push_back(kConnectionError);
        return false;
    }

    bool received_search_res = false;
    bool can_read_more = true;

    while (true) {

        if (can_read_more) result->push_back(wstring());

        can_read_more = ReadMessageFromPipe(data_pipe, result->back());

        if (result->back().size() > 1 && result->back() == kLastMessageIndicator) {
            received_search_res = true;
            result->pop_back();
            break;
        }
    }

    return received_search_res;
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
    return CreateFile((L"\\\\.\\pipe\\" + pipename).c_str(),  // pipe name
                      GENERIC_READ | GENERIC_WRITE,           // read/write access
                      0,                                      // no sharing
                      NULL,                                   // default security attributes
                      OPEN_EXISTING,                          // opens existing pipe
                      0,                                      // default attributes
                      NULL);                                  // no template file
}

void ConnectionManager::ParseData(const wstring& data, wstring* pipename, wstring* query, wstring* format,
                                  int* max_files) {

    auto parts = HelperCommon::Split(data, delimeter, HelperCommon::SplitOptions::INCLUDE_EMPTY);
    *pipename  = parts[0];
    *query     = parts[1];
    *format    = parts[2];
    *max_files = HelperCommon::ParseNumber<int>(parts[3]);
}

bool ConnectionManager::WriteMessageToPipe(HANDLE pipe, const wstring& message) {
    DWORD bytes_written;
    return WriteFile(pipe, message.c_str(), 2 * message.size(), &bytes_written, NULL /* not overlapped */) != 0;
}

bool ConnectionManager::ReadMessageFromPipe(HANDLE pipe, wstring& result) {
    DWORD bytes_read;
    bool ok = ReadFile(pipe, buffer_, kBufSize * sizeof(TCHAR), &bytes_read, NULL /*not overlapped */) != 0;
    result = wstring(buffer_, bytes_read / 2);
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