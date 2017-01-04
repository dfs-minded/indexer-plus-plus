// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "RawMftReader.h"

#include <algorithm>

#include "CommandlineArguments.h"
#include "FileInfo.h"
#include "../Common/Helpers.h"
#include "Log.h"

#include "RawMFTRecordsParser.h"
#include "RawMFTSerializer.h"
#include "TestFramework/FileInfoObjectsSerializer.h"
#include "WinApiCommon.h"

namespace ntfs_reader {

	using std::unique_ptr;
	using std::make_unique;
	using std::vector;

	using namespace indexer_common;

	const uint RawMFTReader::kBufferSize = 1024 * 1024;

    RawMFTReader::RawMFTReader(char drive_letter)
        : volume_(nullptr),
          u_records_parser_(nullptr),
          read_serialized_mft_(false),
		  save_raw_mft_(CommandlineArguments::Instance().SaveRawMFT) {

		auto serialiezed_mft_path = CommandlineArguments::Instance().RawMFTPath;
        read_serialized_mft_ = !serialiezed_mft_path.empty();
#ifdef WIN32
        if (!read_serialized_mft_) {
            volume_ = unique_ptr<void, std::function<void(HANDLE)>>(WinApiCommon::OpenVolume(drive_letter),
                                                               [](HANDLE volume) { CloseHandle(volume); });
        }
#endif

        NTFS_VOLUME_DATA_BUFFER volume_data_buff;
        if (GetNtfsVolumeData(&volume_data_buff)) {
            volume_data_ = VolumeData(drive_letter, volume_data_buff);
            u_records_parser_ = make_unique<RawMFTRecordsParser>(volume_data_);
        }
    }

    bool RawMFTReader::GetNtfsVolumeData(NTFS_VOLUME_DATA_BUFFER* volume_data_buff) const {

        bool ok = read_serialized_mft_ ? RawMFTSerializer::Instance().GetNtfsVolumeData(volume_data_buff)
                                       : WinApiCommon::GetNtfsVolumeData(volume_.get(), volume_data_buff);

        if (!ok) {
			helpers::WriteToOutput(METHOD_METADATA + helpers::GetLastErrorString());
            return false;
        }

        if (save_raw_mft_) {
            RawMFTSerializer::Instance().SerializeMFT((void*)volume_data_buff, sizeof(*volume_data_buff));
        }

        return true;
    }

	void RawMFTReader::SetVolumePointerFromVolumeBegin(uint64 bytes_to_move) const {
        if (read_serialized_mft_) return;

        WinApiCommon::SetFilePointerFromFileBegin(volume_.get(), bytes_to_move);
    }

	using std::pair;

	unique_ptr<vector<pair<int64, int64>>> RawMFTReader::GetMFTRetrievalPointers(char* buff) const {

        int res = read_serialized_mft_ ? RawMFTSerializer::Instance().ReadMFTFromFile(buff, volume_data_.MFTRecordSize)
                                       : WinApiCommon::ReadBytes(volume_.get(), buff, volume_data_.MFTRecordSize);

        if (!res) {
			helpers::WriteToOutput(METHOD_METADATA + helpers::GetLastErrorString());
            return nullptr;
        }

        if (save_raw_mft_) {
            RawMFTSerializer::Instance().SerializeMFT(buff, volume_data_.MFTRecordSize);
        }

        auto poiners = u_records_parser_->GetMFTRetrievalPointers(buff);
        return poiners;
    }

	unique_ptr<MFTReadResult> RawMFTReader::ReadAllRecords() {

		auto u_result = make_unique<MFTReadResult>();

        auto u_buff = make_unique<char[]>(kBufferSize);

        SetVolumePointerFromVolumeBegin(volume_data_.MFTStartLCN * volume_data_.BytesPerCluster);

        auto u_retr_pointers = GetMFTRetrievalPointers(u_buff.get());
        if (!u_retr_pointers) return u_result;

        // Main loop via MFT pointers (for fragmented MFT data). If the MFT located in one piece of memory this loop will
        // iterate only once. pair<run_offset, run_length>
        for (const auto& pair : *u_retr_pointers) {
			uint64 reading_offset = 0;

			uint64 run_offset = pair.first, run_length = pair.second;

            SetVolumePointerFromVolumeBegin(run_offset);

            while (reading_offset < run_length) {
				auto bytes_count = static_cast<uint>(std::min((uint64)kBufferSize, volume_data_.MFTSize - reading_offset));

                int res = read_serialized_mft_ ? RawMFTSerializer::Instance().ReadMFTFromFile(u_buff.get(), bytes_count)
                                               : WinApiCommon::ReadBytes(volume_.get(), u_buff.get(), bytes_count);

                if (!res) return u_result;

                if (save_raw_mft_) {
                    RawMFTSerializer::Instance().SerializeMFT(u_buff.get(), bytes_count);
                }

                reading_offset += bytes_count;
                auto curr_records_num_to_parse = bytes_count / volume_data_.MFTRecordSize;

                u_records_parser_->Parse(u_buff.get(), curr_records_num_to_parse);
            }
        }

        auto u_accumulated_file_infos = u_records_parser_->GetCompleteFileInfos();

        FindAndSetRoot(*u_accumulated_file_infos, u_result.get(), volume_data_.DriveLetter);

		if (CommandlineArguments::Instance().SaveFileInfos) {
            for (auto* fi : *u_accumulated_file_infos) {
                if (fi) FileInfoObjectsSerializer::Instance().SerializeFileInfoToFile(*fi);
            }
        }

        u_result->Data = move(u_accumulated_file_infos);
        return u_result;
    }

	void RawMFTReader::FindAndSetRoot(const vector<FileInfo*>& file_infos, MFTReadResult* result, char drive_letter) {

        for (auto* fi : file_infos) {
            if (!fi) continue;

            if (fi->ID == fi->ParentID) {
				fi->SetName(helpers::GetDriveName(drive_letter), 2);
                fi->Parent = fi;
                result->Root = fi;
                break;
            }
        }
    }

} // namespace ntfs_reader