#pragma once

#include "Macros.h"

// Used for debug log window.
class LogMessagesListener {
   public:
    NO_COPY(LogMessagesListener)

    LogMessagesListener() = default;

    virtual ~LogMessagesListener() = default;

    virtual void OnNewMessage(const std::wstring& msg) const = 0;
};
