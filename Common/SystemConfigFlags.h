#pragma once

#include "Macros.h"

// Wrapper on CompilerSymbols.h to make it reachable for C++/CLI classes.
// All defines must be made only in CompilerSymbols.h file.

class SystemConfigFlags {
   public:
    static SystemConfigFlags& Instance();

    NO_COPY(SystemConfigFlags)

    bool CallWatchChanges;
    bool TrayIcon;
    bool PipeManager;
    bool ShelContextMenu;
    bool ShowDebugLogWindow;

   private:
    SystemConfigFlags();
};
