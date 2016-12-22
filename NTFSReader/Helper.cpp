// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "Helper.h"

#include <string.h>
#include <vector>

#include "WindowsWrapper.h"

#include "HelperCommon.h"

namespace ntfs_reader {

    using namespace std;

    const wstring Helper::kDelim = L":";

    const pair<wstring, DWORD> Helper::kUSNReasonsPairs[] = {

        make_pair(wstring(L"USN_REASON_DATA_OVERWRITE"), 0x00000001),
        make_pair(wstring(L"USN_REASON_DATA_EXTEND"), 0x00000002),
        make_pair(wstring(L"USN_REASON_DATA_TRUNCATION"), 0x00000004),
        make_pair(wstring(L"USN_REASON_NAMED_DATA_OVERWRITE"), 0x00000010),
        make_pair(wstring(L"USN_REASON_NAMED_DATA_EXTEND"), 0x00000020),
        make_pair(wstring(L"USN_REASON_NAMED_DATA_TRUNCATION"), 0x00000040),
        make_pair(wstring(L"USN_REASON_FILE_CREATE"), 0x00000100),
        make_pair(wstring(L"USN_REASON_FILE_DELETE"), 0x00000200),
        make_pair(wstring(L"USN_REASON_EA_CHANGE"), 0x00000400),
        make_pair(wstring(L"USN_REASON_SECURITY_CHANGE"), 0x00000800),
        make_pair(wstring(L"USN_REASON_RENAME_OLD_NAME"), 0x00001000),
        make_pair(wstring(L"USN_REASON_RENAME_NEW_NAME"), 0x00002000),
        make_pair(wstring(L"USN_REASON_INDEXABLE_CHANGE"), 0x00004000),
        make_pair(wstring(L"USN_REASON_BASIC_INFO_CHANGE"), 0x00008000),
        make_pair(wstring(L"USN_REASON_HARD_LINK_CHANGE"), 0x00010000),
        make_pair(wstring(L"USN_REASON_COMPRESSION_CHANGE"), 0x00020000),
        make_pair(wstring(L"USN_REASON_ENCRYPTION_CHANGE"), 0x00040000),
        make_pair(wstring(L"USN_REASON_OBJECT_ID_CHANGE"), 0x00080000),
        make_pair(wstring(L"USN_REASON_REPARSE_POINT_CHANGE"), 0x00100000),
        make_pair(wstring(L"USN_REASON_STREAM_CHANGE"), 0x00200000),
        make_pair(wstring(L"USN_REASON_TRANSACTED_CHANGE"), 0x00400000),
        make_pair(wstring(L"USN_REASON_INTEGRITY_CHANGE"), 0x00800000),
        make_pair(wstring(L"USN_REASON_CLOSE"), 0x80000000)};


    wstring Helper::SerializeRecord(const USN_RECORD& record, char drive) {
        wstring res;
        res += drive;  // 0
        res += kDelim;
        res += to_wstring(record.RecordLength) + kDelim;               // 1
        res += to_wstring(record.MajorVersion) + kDelim;               // 2
        res += to_wstring(record.MinorVersion) + kDelim;               // 3
        res += to_wstring(record.FileReferenceNumber) + kDelim;        // 4
        res += to_wstring(record.ParentFileReferenceNumber) + kDelim;  // 5
        res += to_wstring(record.Usn) + kDelim;                        // 6

        res += to_wstring(record.TimeStamp.HighPart) + kDelim;  // 7
        res += to_wstring(record.TimeStamp.LowPart) + kDelim;   // 8

        res += to_wstring(record.Reason) + kDelim;       // 9
        res += GetReasonString(record.Reason) + kDelim;  // Human friendly representation, not used in parsing.
        res += to_wstring(record.SourceInfo) + kDelim;   // 11
        res += to_wstring(record.SecurityId) + kDelim;   // 12

        res += to_wstring(record.FileAttributes) + kDelim;  // 13
        res += to_wstring(record.FileNameLength) + kDelim;  // 14
        res += to_wstring(record.FileNameOffset) + kDelim;  // 15

        ushort length;
        res += reinterpret_cast<const wchar_t*>(HelperCommon::GetFilename(record, &length));  // 16
        res += kDelim;

        return move(res);
    }

    pair<unique_ptr<USN_RECORD, function<void(USN_RECORD*)>>, char> Helper::DeserializeRecord(const wstring& s) {

        auto parts = HelperCommon::Split(s, kDelim);
        auto filename_length = HelperCommon::ParseNumber<int>(parts[14]);

        if (filename_length != 2 * parts[16].size()) {
            auto size = parts[16].size();
            WriteToOutput(string("Incorrect filename size: ") + to_string(size));
        }

        auto record_size = HelperCommon::ParseNumber<int>(parts[1]);

        // Malloc is needed to hold record in one memory block, not to split it. This made for the convenience to put
        // everything in one buffer (not to have separate buffer for other memory piece for FileName property).
        unique_ptr<USN_RECORD, function<void(USN_RECORD*)>> record(static_cast<USN_RECORD*>(malloc(record_size)),
                                                                   [](USN_RECORD* r) { free(r); });

        record->RecordLength              = record_size;
        record->MajorVersion              = HelperCommon::ParseNumber<WORD>(parts[2]);
        record->MinorVersion              = HelperCommon::ParseNumber<WORD>(parts[3]);
        record->FileReferenceNumber       = HelperCommon::ParseNumber<DWORDLONG>(parts[4]);
        record->ParentFileReferenceNumber = HelperCommon::ParseNumber<DWORDLONG>(parts[5]);
        record->Usn                       = HelperCommon::ParseNumber<USN>(parts[6]);

        record->TimeStamp.HighPart = HelperCommon::ParseNumber<LONG>(parts[7]);
        record->TimeStamp.LowPart = HelperCommon::ParseNumber<DWORD>(parts[8]);

        record->Reason = HelperCommon::ParseNumber<DWORD>(parts[9]);
        record->SourceInfo = HelperCommon::ParseNumber<DWORD>(parts[11]);
        record->SecurityId = HelperCommon::ParseNumber<DWORD>(parts[12]);

        record->FileAttributes = HelperCommon::ParseNumber<DWORD>(parts[13]);
        record->FileNameLength = filename_length;
        record->FileNameOffset = HelperCommon::ParseNumber<int>(parts[15]);


        memcpy(record->FileName, parts[16].c_str(), filename_length);

        auto drive = static_cast<char>(parts[0][0]);

        return make_pair(move(record), drive);
    }

    wstring Helper::GetReasonString(DWORD reason) {
        wstring res;
        for (auto r : kUSNReasonsPairs) {
            if (r.second & reason) res += r.first + L" & ";
        }
        return res == L"" ? L" " : res;
    }

    wstring Helper::GetEncodingString(const wchar_t* text) {
        wstring res;
        while (*text != 0) {
            res += to_wstring((int)*text) + L", ";
            text++;
        }
        return res;
    }

} // namespace ntfs_reader