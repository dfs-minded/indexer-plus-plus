// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "Serializer.h"

#include <fcntl.h>
#include <sstream>

#include "IndexerDateTime.h"

namespace ntfs_reader {

	using std::wstring;
	using std::to_wstring;

	using indexer_common::uint64;
	using indexer_common::IndexerDateTime;


    const wstring Serializer::kDelim = L";";

    Serializer::Serializer() {
#ifdef WIN32
        // string fname = string("MFTSerialized") + to_string(IndexerDateTime::TicksNow()) + string(".csv");

        std::ostringstream fname;
		fname << "MFTSerialized" << IndexerDateTime::TicksNow() << ".csv";

        fopen_s(&mft_serialization_file_, fname.str().c_str(), "w");
        _setmode(_fileno(mft_serialization_file_), _O_U8TEXT);

	/*wstring header = L"Signature" + delim + L"Lsn" + delim + L"SequenceNumber" + delim +
                                     L"ReferenceCount" + delim + L"Flags" + delim + L"FirstFreeByte" + delim +
                                     L"MFTRecordNumber" + delim + L"ParentID" + delim + L"SizeReal" + delim +
                                     L"SizeAllocated" + delim + L"Flags" + delim + L"FileName";

    WriteToFile(header);
    Endl();*/
#endif
    }

    Serializer::~Serializer() {
        fflush(mft_serialization_file_);
        fclose(mft_serialization_file_);
    }

    const Serializer* Serializer::Serialize(P_FILE_RECORD_HEADER record_header) {
        auto resptr = new wstring();
        auto& res = *resptr;

        auto sign = *record_header->MultiSectorHeader.Signature;

		res += to_wstring((indexer_common::uchar)sign) + kDelim;
        res += to_wstring(record_header->LogSeqNumber) + kDelim;
        res += to_wstring(record_header->SequenceNumber) + kDelim;
        res += to_wstring(record_header->HardLinks) + kDelim;
        res += to_wstring(record_header->Flags) + kDelim;
        res += to_wstring(record_header->RecLength) + kDelim;
        res += to_wstring(record_header->MFTRecordNumber);

        WriteToFile(res);
        return this;
    }

    const Serializer* Serializer::Serialize(const std::wstring& data) {
        WriteToFile(data);
        return this;
    }

	const Serializer* Serializer::Serialize(const indexer_common::uint64 data) {
        WriteToFile(to_wstring(data));
        return this;
    }

    const Serializer* Serializer::Endl() {
        WriteToFile(L"\n");
        return this;
    }

    const Serializer* Serializer::Delim() {
        WriteToFile(kDelim);
        return this;
    }

    void Serializer::WriteToFile(const wstring& data) const {
        fwprintf(mft_serialization_file_, L"%s", data.c_str());
        fflush(mft_serialization_file_);
    }

} // namespace ntfs_reader