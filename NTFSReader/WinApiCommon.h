// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <string>

#include "WindowsWrapper.h"

class FileInfo;


// Wraps common function calls to WinAPI.

class WinApiCommon {
   public:
    // Opening physical drive for reading.

    static HANDLE OpenVolume(char drive_letter);


    static HANDLE CreateFileForWrite(const std::wstring& filename);


    static HANDLE OpenFileForRead(const std::wstring& filename);


    // Wrapper on the winAPI ReadFile function.

    static bool ReadBytes(HANDLE h_file, LPVOID buffer, DWORD bytes_to_read);


    // Wrapper on the winAPI WriteFile function.

    static void WriteBytes(HANDLE h_file, LPVOID buffer, DWORD bytes_to_write);


    // Sets the file pointer on the specified distance, starting from its beginning.

    static void SetFilePointerFromFileBegin(HANDLE volume, ULONGLONG bytes_to_move);


    static bool GetNtfsVolumeData(HANDLE h_file, NTFS_VOLUME_DATA_BUFFER* volume_data_buff);


    // Queries the current update sequence number (USN) change journal, its records, and its capacity
    // and fills empty |journal_data|, provided by the caller.

    static bool LoadJournal(HANDLE volume, USN_JOURNAL_DATA* journal_data);


    // Reads file attributes - size and timestamps and updates with them corresponding members of |file_info|.
    // This function can be called only when |file_info| full filename can be retrieved.
    // |path| is the path of the |file_info|.

    static bool GetSizeAndTimestamps(const wchar_t& path, FileInfo* file_info);

   private:
    // Creates an update sequence number (USN) change journal stream on a target volume, or modifies
    // an existing change journal stream.

    static bool CreateJournal(HANDLE volume);
};
