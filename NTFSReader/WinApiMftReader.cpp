// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "WinApiMftReader.h"

#include "FileInfo.h"
#include "HelperCommon.h"
#include "typedefs.h"

#include "VolumeData.h"
#include "WinApiCommon.h"

namespace ntfs_reader {

    using namespace std;

    const int WinApiMFTReader::kBufferSize = 1024 * 1024 / 2;

    WinApiMFTReader::WinApiMFTReader(char drive_letter)
        : drive_letter_(drive_letter), volume_(WinApiCommon::OpenVolume(drive_letter)) {
    }

    unique_ptr<MFTReadResult> WinApiMFTReader::ReadAllRecords() {

        auto u_result = make_unique<MFTReadResult>();

        auto mft_enum_data = GetMFTEnumData();
        if (!mft_enum_data) return u_result;

        NTFS_VOLUME_DATA_BUFFER volume_data_buff;
        WinApiCommon::GetNtfsVolumeData(volume_, &volume_data_buff);
        auto volume_data = VolumeData(drive_letter_, volume_data_buff);

        DWORD bytecount = 0;
        auto data = make_unique<vector<FileInfo*>>(static_cast<size_t>(volume_data.MFTRecordsNum));
        auto buffer = make_unique<char[]>(kBufferSize);

        while (ReadUSNRecords(mft_enum_data.get(), buffer.get(), bytecount)) {

            mft_enum_data->StartFileReferenceNumber = *(DWORDLONG*)buffer.get();

            auto record = (USN_RECORD*)((USN*)buffer.get() + 1);
            auto recordend = (USN_RECORD*)((BYTE*)buffer.get() + bytecount);

            if (record == recordend) {
                break;
            }

            for (; record < recordend; record = (USN_RECORD*)(((BYTE*)record) + record->RecordLength)) {
                auto fi = new FileInfo(*record, drive_letter_);
                (*data)[fi->ID] = fi;
            }
        }

        auto root = new FileInfo(drive_letter_);

        root->SetName(HelperCommon::GetDriveName(drive_letter_), 2);
        root->ID = FindRootID(*data);

        (*data)[root->ID] = root;

        u_result->Root = root;
        u_result->Data = move(data);

        AssignRootAsParent(*u_result);

        return u_result;
    }

    unique_ptr<MFT_ENUM_DATA> WinApiMFTReader::GetMFTEnumData() const {

        auto u_journal = make_unique<USN_JOURNAL_DATA>();
        WinApiCommon::LoadJournal(volume_, u_journal.get());

        auto mft_enum_data = make_unique<MFT_ENUM_DATA>();

        mft_enum_data->StartFileReferenceNumber = 0;
        mft_enum_data->LowUsn = 0;
        mft_enum_data->HighUsn = u_journal->MaxUsn;
        mft_enum_data->MinMajorVersion = 2;  // journal->MinSupportedMajorVersion;
        mft_enum_data->MaxMajorVersion = 2;  // journal->MaxSupportedMajorVersion;

        return mft_enum_data;
    }


// Enumerates the update sequence number(USN) data between two specified boundaries to obtain MFT records.

    bool WinApiMFTReader::ReadUSNRecords(PMFT_ENUM_DATA mft_enum_data, LPVOID buffer, DWORD& byte_count) const {
#ifdef WIN32
        return DeviceIoControl(volume_,                 // handle to volume
                               FSCTL_ENUM_USN_DATA,     // dwIoControlCode
                               mft_enum_data,           // input buffer
                               sizeof(*mft_enum_data),  // size of input buffer
                               buffer,                  // output buffer
                               kBufferSize,             // size of output buffer
                               &byte_count,             // number of bytes returned
                               NULL) != 0;              // OVERLAPPED structure
#else                                               // TODO Linux?
        return true;
#endif
    }


// Since WinApi does not give record of the root directory, we need to create it ourselves.
// First we want to determine root ID, relaying on the assumption, that there is a not hidden FileInfo
// in the map, that has ParentID which does not correspond to any FileInfo in the map.
// We take for investigation only not hidden files (user created files), because hidden system files
// most likely have other system file as a parent.

    uint WinApiMFTReader::FindRootID(const vector<FileInfo*>& data) {

        for (const auto* fi : data) {
            if (!fi) continue;

            // File not hidden and do not have already parent in the vector.
            if (!fi->IsHiddenOrSystem() && !data[fi->ParentID]) {
                return fi->ParentID;
            }
        }

        return 0;
    }


// WinApi does not provide particular system files, namely system metadata files, that could be root
// files for other, provided by WinApi system files. As a result, this files do not have corresponding
// parent FileInfo, and for consistency we want to assign the root FileInfo as their parent.

    void WinApiMFTReader::AssignRootAsParent(const MFTReadResult& read_res) {

        for (auto* fi : *read_res.Data) {
            if (!fi) continue;

            if (!(*read_res.Data)[fi->ParentID]) fi->ParentID = read_res.Root->ID;
        }
    }

} // namespace ntfs_reader