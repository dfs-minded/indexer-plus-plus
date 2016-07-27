#pragma once

#include <list>
#include <string>

#include "Log.h"
#include "Macros.h"

#ifndef SINGLE_THREAD
// This is needed for compatibility with C++/CLI classes.
// <mutex> and <thread> are not supported when compiling with /clr or /clr:pure.
namespace std {
class mutex;
class thread;
}
#endif

class AsyncLog : public Log {
   public:
    static Log& Instance();

    NO_COPY(AsyncLog)

    virtual void Debug(const std::wstring& message) override;

    virtual void Info(const std::wstring& message) override;

    virtual void Warning(const std::wstring& message) override;

    virtual void Error(const std::wstring& message) override;

   private:
    AsyncLog();

    ~AsyncLog();

    void WriteToFile();

    FILE* log_file_;

    std::list<std::wstring> messages_;

    std::list<std::wstring> tmp_messages_storage_;

#ifndef SINGLE_THREAD
    std::mutex* locker_;
    std::thread* worker_thread_;
#endif
};
