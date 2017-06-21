// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

namespace indexer_common {

//-------------------------------------------------------------------------
// Compilation symbols
//-------------------------------------------------------------------------

#define TRAY_ICON           // Enables windows tray icon support for the application.
#define PIPE_MANAGER        // For interprocess communication, to prevent from starting one more instance of app (if it
							// already runs).

#define SHELL_CONTEXT_MENU  // Enables on file right click context menu, the same as in Windows Explorer.
#define SHOW_DEBUG_LOG_WINDOW

#define COMMANDLINE_SERVER

#define WATCH_CHANGES  // Indicates if we want to listen file system changes from USN Journal.

//#define SINGLE_THREAD_LOG // Writes log synchronously, useful for debug. By defaul logging is off.
//#define ASYNC_LOG // Writes log asynchronously, useful for debug. By defaul logging is off.

//#define SINGLE_THREAD // Test framework works only for single thread mode.

//#define WIN_API_MFT_READ // Reads MFT using WinAPI. Fallback API for testing and debugging purposes.

// !!! *** *** ***
// For other options use commandline arguments, which you can find in CmdArgumentsParser class.

} // namespace indexer_common