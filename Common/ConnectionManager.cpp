#include "ConnectionManager.h"

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "WindowsWrapper.h"

#include "HelperCommon.h"

using namespace std;

const wchar_t ConnectionManager::kDefaultPipeName[] = L"Indexer++pipe";

const wstring ConnectionManager::kLastMessageIndicator = L"|";

#ifdef WIN32
void ConnectionManager::CreateServer(pIQueryProcessor queryProcessor) {
    query_processor_ = queryProcessor;
    thread worker(&ConnectionManager::ServerMain, *this);
    worker.detach();
}

void ConnectionManager::ServerMain() {
    for (;;) {
        pipe_ = CreatePipe(kDefaultPipeName);
        if (pipe_ == INVALID_HANDLE_VALUE) {
            cout << "Invalid pipe " << GetLastError() << endl;
            return;
        }

        // DWORD bytes_read;
        bool connected = ConnectNamedPipe(pipe_, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
        wstring message;
        ReadMessageFromPipe(pipe_, message);
        if (message.empty()) continue;
        wstring data_pipename, query, format;
        int max_files;
        ParseData(message, &data_pipename, &query, &format, &max_files);
        thread replayRunner(&ConnectionManager::Reply, *this, query, format, data_pipename, max_files);
        replayRunner.detach();

        CloseHandle(pipe_);
    }
}

const wstring delimeter = L";";
const wchar_t* kConnectionError = L"Error connecting to Indexer++ process. May be the process was not started or cmd client was not run as administrator.";

bool ConnectionManager::SendQuery(const std::wstring& query, const std::wstring& format, int max_files,
                                  std::vector<std::wstring>* result) {
    HANDLE main_pipe       = OpenPipe(kDefaultPipeName);
    wstring data_pipe_name = L"indexer" + to_wstring(GetCurrentProcessId());
    HANDLE data_pipe       = CreatePipe(data_pipe_name);

    wstring message = data_pipe_name + delimeter + query + delimeter + format + delimeter + to_wstring(max_files);

    bool ok = WriteMessageToPipe(main_pipe, message);
    if (!ok) {
        result->push_back(kConnectionError);
        return false;
    }

    // bool connected = ConnectNamedPipe(data_pipe, nullptr) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
    bool connected = ConnectNamedPipe(data_pipe, nullptr);
    if (!connected) {
        result->push_back(kConnectionError);
        return false;
    }
    for (;;) {
        result->push_back(wstring());
        bool ok = ReadMessageFromPipe(data_pipe, result->back());
        if (result->back().size() > 0 && result->back() == kLastMessageIndicator) break;
    }
    result->resize(result->size() - 1);
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
    return CreateFile((L"\\\\.\\pipe\\" + pipename).c_str(),  // pipe name
                      GENERIC_READ | GENERIC_WRITE,
                      0,              // no sharing
                      NULL,           // default security attributes
                      OPEN_EXISTING,  // opens existing pipe
                      0,              // default attributes
                      NULL);          // no template file
}

void ConnectionManager::ParseData(const std::wstring& data, std::wstring* pipename, std::wstring* query,
                                  std::wstring* format, int* max_files) {
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
    result  = wstring(buffer_, bytes_read / 2);
    return true;  // TODO error handling.
}

void ConnectionManager::Reply(const wstring& query, const wstring& format, const wstring& data_pipename,
                              int max_files) {
    vector<wstring> result = query_processor_->Process(query, format, max_files);
    HANDLE data_pipe      = OpenPipe(data_pipename);
    for (const wstring& s : result)
        WriteMessageToPipe(data_pipe, s);
    WriteMessageToPipe(data_pipe, kLastMessageIndicator);  // Last message.
    CloseHandle(data_pipe);
}
#else
void ConnectionManager::CreateServer(pIQueryProcessor queryProcessor) {
}

void ConnectionManager::ServerMain() {
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