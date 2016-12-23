// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "RawMFTSerializer.h"

#include <string.h>
#include <fstream>

#include "CommandlineArguments.h"
#include "IndexerDateTime.h"

#include "../Common/Helpers.h"
#include "WinApiCommon.h"

namespace ntfs_reader {

	using namespace indexer_common;

    RawMFTSerializer::RawMFTSerializer() {

		if (CommandlineArguments::Instance().SaveRawMFT) {
			auto filename = L"RawMFT_" + std::to_wstring(IndexerDateTime::TicksNow()) + L".txt";

            raw_mft_file_ = WinApiCommon::CreateFileForWrite(filename);
        }

		auto serialiezed_mft_path = CommandlineArguments::Instance().RawMFTPath;

        if (!serialiezed_mft_path.empty()) {

#ifdef WIN32
            // raw_mft_file_ = WinApiCommon::OpenFileForRead(serialiezed_mft_path);
            serialized_raw_mft_file_ = std::make_unique<std::ifstream>(serialiezed_mft_path, std::ifstream::binary);
#else
            //  string file_path = HelperCommon::WStringToString(serialiezed_mft_path);
            //  WriteToOutput("*** " + file_path);
            // ifstream* ifs = new ifstream(file_path.c_str(), ios::binary);
            // serialized_raw_mft_file_.reset(ifs);
            serialized_raw_mft_file_ =
                make_unique<ifstream>(helpers::WStringToString(serialiezed_mft_path), ifstream::binary);
#endif
        }
    }

    RawMFTSerializer::~RawMFTSerializer() {
#ifdef WIN32
        CloseHandle(raw_mft_file_);
#endif
    }

    RawMFTSerializer& RawMFTSerializer::Instance() {
        static RawMFTSerializer instance_;
        return instance_;
    }

    void RawMFTSerializer::SerializeMFT(void* buff, DWORD bytes_count) const {
        WinApiCommon::WriteBytes(raw_mft_file_, buff, bytes_count);
    }

    bool RawMFTSerializer::GetNtfsVolumeData(NTFS_VOLUME_DATA_BUFFER* volume_data_buff) {

        DWORD volume_data_size = sizeof(*volume_data_buff);
        auto u_buff = std::make_unique<char[]>(volume_data_size);

        // bool ok = WinApiCommon::ReadBytes(raw_mft_file_, u_buff.get(), volume_data_size);
        bool ok = serialized_raw_mft_file_->read((char*)u_buff.get(), volume_data_size) ? true : false;

        if (ok) {
            memcpy(volume_data_buff, u_buff.get(), volume_data_size);
        }
        return ok;
    }

    bool RawMFTSerializer::ReadMFTFromFile(void* buffer, DWORD bytes_to_read) {
        serialized_raw_mft_file_->read((char*)buffer, bytes_to_read);
        return serialized_raw_mft_file_ ? true : false;
        // return WinApiCommon::ReadBytes(raw_mft_file_, buffer, bytes_to_read);
    }

} // namespace ntfs_reader