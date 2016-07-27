// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "WindowsWrapper.h"

#ifndef WIN32
bool FileTimeToLocalFileTime(const FILETIME *lpFileTime, LPFILETIME lpLocalFileTime) {
    lpLocalFileTime->dwHighDateTime = lpLocalFileTime->dwLowDateTime = 0;
    return true;
}

void GetLocalTime(LPSYSTEMTIME lpSystemTime) {
    lpSystemTime->wYear  = 1970;
    lpSystemTime->wMonth = lpSystemTime->wDay = 1;
    lpSystemTime->wHour = lpSystemTime->wMinute = lpSystemTime->wSecond = lpSystemTime->wMilliseconds = 0;
}

bool SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, LPFILETIME lpFileTime) {
    lpFileTime->dwHighDateTime = lpFileTime->dwLowDateTime = 0;
    return true;
}

bool FileTimeToSystemTime(const FILETIME *lpFileTime, LPSYSTEMTIME lpSystemTime) {
    lpSystemTime->wYear  = 1970;
    lpSystemTime->wMonth = lpSystemTime->wDay = 1;
    lpSystemTime->wHour = lpSystemTime->wMinute = lpSystemTime->wSecond = lpSystemTime->wMilliseconds = 0;
    return true;
}

ULONGLONG GetTickCount64(void) {
    return 0;
}

DWORD GetTickCount() {
    return 0;
}

void Sleep(DWORD dwMilliseconds) {
    // TODO
}
#endif