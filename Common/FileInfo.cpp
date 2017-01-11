// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "FileInfo.h"

#include <cstring>

#include "WindowsWrapper.h"

#include "Helpers.h"

namespace indexer_common {

    using namespace std;

	FileInfo::FileInfo(char drive_letter)
        : ID(0),
          ParentID(0),

          DriveLetter(drive_letter),
          NameLength(0),
          FileAttributes(0),

          SizeReal(0),
          // SizeAllocated(0),

          CreationTime(0),
          LastAccessTime(0),
          LastWriteTime(0),

          Parent(nullptr),
          FirstChild(nullptr),
          PrevSibling(nullptr),
          NextSibling(nullptr),
          name_(nullptr) {
    }

    FileInfo::FileInfo(const USN_RECORD& record, char drive_letter) : FileInfo(drive_letter) {
        UpdateFromRecord(record);
    }

    FileInfo::~FileInfo() {
        delete[] name_;
    }

    void FileInfo::UpdateFromRecord(const USN_RECORD& record) {
        ID = record.FileReferenceNumber & 0x00000000FFFFFFFF;
        ParentID = record.ParentFileReferenceNumber & 0x00000000FFFFFFFF;
        FileAttributes = record.FileAttributes;

        ushort name_len;
		auto name = helpers::GetFilename(record, &name_len);

        SetName(name, name_len);
    }

    void FileInfo::SetName(const char16_t* name, ushort name_length) {
        delete[] name_;
        // name_ = L""; return;

        name_ = name;
        NameLength = name_length;
    }

    void FileInfo::CopyAndSetName(const char16_t* name, ushort name_length) {
        // name_ = L""; return;
        char16_t* filename_copy = new char16_t[name_length + 1];

        memcpy(filename_copy, name, 2 * name_length);

        *(filename_copy + name_length) = L'\0';

        SetName(filename_copy, name_length);
    }

// Checks if the file name starts by $ sign (which means that it is hidden) or one of the attributes
// says that the file is hidden.
    bool FileInfo::IsHiddenOrSystem() const {
        if (GetName()[0] == '$') return true;

        if ((FileAttributes & FILE_ATTRIBUTE_SYSTEM) || (FileAttributes & FILE_ATTRIBUTE_VIRTUAL) ||
            (FileAttributes & FILE_ATTRIBUTE_DEVICE) || (FileAttributes & FILE_ATTRIBUTE_HIDDEN)) {
            return true;
        }

        return false;
    }

    bool FileInfo::IsDirectory() const {
        return FileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    }

} // namespace indexer_common
