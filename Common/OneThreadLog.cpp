#include "OneThreadLog.h"

#include <fcntl.h>
#include "WindowsWrapper.h"

#include "LogMessagesListener.h"

using namespace std;

Log& OneThreadLog::Instance() {
    static OneThreadLog instance;
    return instance;
}

OneThreadLog::OneThreadLog() {
#ifdef WIN32
    log_file_ = _wfopen(L"IndexerDebugLog.txt", L"w");
    _setmode(_fileno(log_file_), _O_U8TEXT);
#else
    log_file_ = fopen("IndexerDebugLog.txt", "w");
#endif
}

OneThreadLog::~OneThreadLog() {
    fclose(log_file_);
}

void OneThreadLog::Debug(const wstring& message) {
    COMPOSE_MSG(L"DEBUG");
    WriteToFile(msg);
}

void OneThreadLog::Info(const wstring& message) {
    COMPOSE_MSG(L"INFO");
    WriteToFile(msg);
}

void OneThreadLog::Warning(const wstring& message) {
    COMPOSE_MSG(L"WARNING");
    WriteToFile(msg);
}

void OneThreadLog::Error(const wstring& message) {
    COMPOSE_MSG(L"ERROR");
    WriteToFile(msg);
}

void OneThreadLog::WriteToFile(wstring& msg) {
    for (auto listener : messages_listeners_) {
        listener->OnNewMessage(msg);
    }

    fwprintf(log_file_, L"%s\n", msg.c_str());
    fflush(log_file_);
}
