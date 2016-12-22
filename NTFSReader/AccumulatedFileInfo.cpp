// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "AccumulatedFileInfo.h"

#include "HelperCommon.h"
#include "IndexerDateTime.h"

namespace ntfs_reader {

    using namespace std;

    AccumulatedFileInfo::AccumulatedFileInfo(char drive_letter, uint id)
        : UsedFilenameFlags(FilenameFlagsEnum::EMPTY),
          // HardLinksCount(0),
          SizeReal(0),
          // SizeAllocated(0),
          fi_(make_unique<FileInfo>(drive_letter)) {

        fi_->ID = id;
    }

    AccumulatedFileInfo::~AccumulatedFileInfo() {
        fi_ = nullptr;
    }

    void AccumulatedFileInfo::SetTimestamps(const STANDARD_INFORMATION& std_info) const {

        fi_->CreationTime   = IndexerDateTime::WindowsTicksToUnixSeconds(std_info.CreationTime);
        fi_->LastAccessTime = IndexerDateTime::WindowsTicksToUnixSeconds(std_info.LastAccessTime);
        fi_->LastWriteTime  = IndexerDateTime::WindowsTicksToUnixSeconds(std_info.LastModificationTime);
    }

    bool AccumulatedFileInfo::SetParentID(uint64 parent_id, FilenameFlagsEnum flags) const {
        if (!parent_id) return false;

        if (!fi_->ParentID || NewFlagsBetterThanUsed(flags)) {
            fi_->ParentID = (parent_id & 0x00000000FFFFFFFF);
            return true;
        }

        return false;
    }

    bool AccumulatedFileInfo::NewFlagsBetterThanUsed(FilenameFlagsEnum new_flags) const {

        if (UsedFilenameFlags == FilenameFlagsEnum::EMPTY) return true;

        if (UsedFilenameFlags == FilenameFlagsEnum::WIN_32 || UsedFilenameFlags == FilenameFlagsEnum::WIN_32_AND_DOS)
            return false;

        return new_flags == FilenameFlagsEnum::WIN_32 || new_flags == FilenameFlagsEnum::WIN_32_AND_DOS;
    }

    bool AccumulatedFileInfo::SetName(const char16_t* name, ushort name_length, FilenameFlagsEnum flags) const {

        if (name == nullptr) return false;

        if (fi_->GetName() == nullptr || NewFlagsBetterThanUsed(flags)) {
            fi_->CopyAndSetName(name, name_length);
            return true;
        }

        return false;
    }

    void AccumulatedFileInfo::SetFileAttributes(ulong file_attributes) const {

        fi_->FileAttributes = file_attributes;

        // File attributes, parsed from NTFS and from USN Journal are differ in "is directory" flag.
        // So we add standard directory flag from <windows.h> for consistency.
        if (fi_->FileAttributes & (int)FilenameFlagsEnum::NTFS_FILE_ATTRIBUTE_DIRECTORY) {
            // Switch off NTFS_FILE_ATTRIBUTE_DIRECTORY flag.
            fi_->FileAttributes &= ~(int)FilenameFlagsEnum::NTFS_FILE_ATTRIBUTE_DIRECTORY;

            fi_->FileAttributes |= (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ARCHIVE);
        }
    }

    bool AccumulatedFileInfo::SetFilenameFlags(FilenameFlagsEnum new_flags) {

        bool better = NewFlagsBetterThanUsed(new_flags);

        if (better) UsedFilenameFlags = new_flags;

        return better;
    }

    unique_ptr<FileInfo> AccumulatedFileInfo::GetFinalResult() {

        if (fi_ && fi_->ParentID && fi_->GetName() != nullptr) {

            fi_->SizeReal = Helper::SizeFromBytesToKiloBytes(SizeReal);
            // fi_->SizeAllocated = SizeAllocated;
            return move(fi_);
        }

        return nullptr;
    }
} // namespace ntfs_reader