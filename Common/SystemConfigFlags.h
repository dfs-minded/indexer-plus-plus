// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

//#include "Macros.h"

// Wrapper on CompilerSymbols.h to make it reachable for C++/CLI classes.
// All defines must be made only in CompilerSymbols.h file.

namespace indexer_common {

    class __declspec(dllexport) SystemConfigFlags {
       public:
        

      //  NO_COPY(SystemConfigFlags)

        bool CallWatchChanges;
        bool TrayIcon;
        bool PipeManager;
        bool ShelContextMenu;
        bool ShowDebugLogWindow;

     //  private:
        SystemConfigFlags();
    };

	__declspec(dllexport) int* SystemConfigFlagsInstance();
} // namespace indexer_common