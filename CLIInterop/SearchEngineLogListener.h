#pragma once

#include <vcclr.h>
#include <string>

#include "DebugLogModel.h"
#include "LogMessagesListener.h"
#include "Macros.h"

class Log;

namespace CLIInterop {
class SearchEngineLogListener : public LogMessagesListener {
   public:
    explicit SearchEngineLogListener(DebugLogModel ^ mdl);

    NO_COPY(SearchEngineLogListener)

    virtual ~SearchEngineLogListener();

    virtual void OnNewMessage(const std::wstring &msg) const override;

   private:
    gcroot<DebugLogModel ^> model;

    Log *logger_;
};
}
