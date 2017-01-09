// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SystemConfigFlags.h"

namespace indexer_common {

    SystemConfigFlags::SystemConfigFlags() {
        TrayIcon = false;
        PipeManager = false;
        CallWatchChanges = false;
        ShelContextMenu = false;
        ShowDebugLogWindow = false;

#ifdef TRAY_ICON
        TrayIcon = true;
#endif

#ifdef PIPE_MANAGER
        PipeManager = true;
#endif

// Tells Main window to call CheckUpdates on DataModel to force updates check in single thread mode.
#if defined(WATCH_CHANGES) && defined(SINGLE_THREAD)
        CallWatchChanges = true;
#endif

#ifdef SHELL_CONTEXT_MENU
        ShelContextMenu = true;
#endif

#ifdef SHOW_DEBUG_LOG_WINDOW
        ShowDebugLogWindow = true;
#endif
    }

	__declspec(dllexport) int* SystemConfigFlagsInstance()
    {
		//static SystemConfigFlags instance;
		//return &instance;
		return nullptr;
    }

} // namespace indexer_common