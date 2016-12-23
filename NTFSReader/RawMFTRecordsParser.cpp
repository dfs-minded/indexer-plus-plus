// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "RawMFTRecordsParser.h"

#include <string.h>
#include <algorithm>
#include <cassert>

#include "FileInfo.h"
#include "../Common/Helper.h"

#include "NTFSDataStructures.h"

namespace ntfs_reader {

	using std::unique_ptr;
	using std::make_unique;
	using std::vector;
	using std::pair;

	using namespace indexer_common;

    RawMFTRecordsParser::RawMFTRecordsParser(VolumeData volume_data)
        : volume_data_(volume_data),
          accum_file_infos_(
              make_unique<vector<AccumulatedFileInfo*>>(static_cast<size_t>(volume_data_.MFTRecordsNum), nullptr)) {
    }

    unique_ptr<vector<pair<int64, int64>>> RawMFTRecordsParser::GetMFTRetrievalPointers(char* buff) const {

        auto* file_record_header = (FILE_RECORD_HEADER*)buff;

        if (!RecordOK(file_record_header)) return nullptr;

        ATTRIBUTE_HEADER* attr_header = nullptr;

        while (GetNextAttrHeader(*file_record_header, attr_header)) {
            if (attr_header->TypeCode == AttributeTypeCodes::DATA) {
                return ParseRunList(*attr_header);
            }
        }

        return nullptr;
    }

	// TODO: fix while constraint.
    unique_ptr<vector<pair<int64, int64>>> RawMFTRecordsParser::ParseRunList(const ATTRIBUTE_HEADER& attr_header) const {
        auto current_run_data_offset = attr_header.Form.Nonresident.LowestVCN;

        auto runlist_first_byte = (char*)&attr_header + attr_header.Form.Nonresident.DatarunOffset;

        uchar four_upper_bits = 0xF0;
        uchar four_lower_bits = 0x0F;

        // List of pair<run_offset, run_length>
        auto u_data_pointers = make_unique<vector<pair<int64, int64>>>();

        while (*runlist_first_byte != 0)  // End of the runs list stream.
        {
            ushort run_offset_field_size = (*runlist_first_byte & four_upper_bits) >> 4;
            ushort run_length_field_size = (*runlist_first_byte & four_lower_bits);

            assert(run_offset_field_size && run_length_field_size && run_offset_field_size <= 4 &&
                   run_length_field_size <= 4);

            auto run_offset =
                GetVariableLengthFieldValue(runlist_first_byte + 1 + run_length_field_size, run_offset_field_size);

            assert(run_offset);

            auto run_length = GetVariableLengthFieldValue(runlist_first_byte + 1, run_length_field_size);

            current_run_data_offset += run_offset;

            u_data_pointers->push_back(std::make_pair(current_run_data_offset * volume_data_.BytesPerCluster,
                                                 run_length * volume_data_.BytesPerCluster));

            runlist_first_byte += 1 + run_length_field_size + run_offset_field_size;
        }  // end of while *runlist_first_byte != 0

        return u_data_pointers;
    }

    void RawMFTRecordsParser::Parse(const char* buffer, uint records_num) const {

        for (uint i = 0; i < records_num; ++i) {
            // Get next MFT record header.
            auto file_record_header = (FILE_RECORD_HEADER*)(buffer + i * volume_data_.MFTRecordSize);

            if (!RecordOK(file_record_header)) {
                ++not_in_use_for_debug_;
                continue;
            }

            auto base_record_id = (uint) * (uint64*)&file_record_header->BaseFileRecordSegment;
            auto frn = GetFRN(*file_record_header);
            auto id = base_record_id ? base_record_id : frn;

            AccumulatedFileInfo* fi_info = (*accum_file_infos_)[id];

            if (fi_info == nullptr) {
                fi_info = new AccumulatedFileInfo(volume_data_.DriveLetter, id);
                // fi_info->HardLinksCount = file_record_header->HardLinks;
                (*accum_file_infos_)[id] = fi_info;
            }

            ParseRecordAttributes(*file_record_header, fi_info);
        }
    }

    void RawMFTRecordsParser::ParseRecordAttributes(const FILE_RECORD_HEADER& file_record_header,
                                                    AccumulatedFileInfo* fi_info) const {
        ATTRIBUTE_HEADER* attr_header = nullptr;

        while (GetNextAttrHeader(file_record_header, attr_header)) {

            switch (attr_header->TypeCode) {

                case AttributeTypeCodes::STANDARD_INFORMATION: {

                    const auto* std_info = (STANDARD_INFORMATION*)GetResidentAttrOffset(*attr_header);

                    fi_info->SetTimestamps(*std_info);

                    break;
                }
                case AttributeTypeCodes::FILENAME: {

                    const auto name_info = (FILE_NAME_INFORMATION*)GetResidentAttrOffset(*attr_header);

                    auto filename_flags = static_cast<FilenameFlagsEnum>(name_info->Flags);

                    fi_info->SetParentID(*(uint64*)&name_info->ParentDirectory, filename_flags);

                    fi_info->SetFileAttributes(name_info->DuplicatedInfo.FileAttributes);

                    fi_info->SetName(reinterpret_cast<const char16_t*>(name_info->Filename), name_info->FilenameLength,
                                     filename_flags);

                    // Filename flags must be set after ParentID and Name.
                    fi_info->SetFilenameFlags(filename_flags);

                    break;
                }
                case AttributeTypeCodes::DATA: {

                    if (!IsDirectory(file_record_header)) {
                        SetFileSizes(fi_info, *attr_header);
                    }

                    break;
                }
                default:
                    break;
            }  // end of switch attr_header->TypeCode.
        }      // end of while GetNextAttrHeader.
    }

    bool RawMFTRecordsParser::IsDirectory(const FILE_RECORD_HEADER& file_record_header) {
        return file_record_header.Flags & static_cast<ushort>(FrhFlagsEnum::FRH_DIRECTORY);
    }

    bool RawMFTRecordsParser::GetNextAttrHeader(const FILE_RECORD_HEADER& record_header,
                                                P_ATTRIBUTE_HEADER& attr_header) const {
        // If was not set before.
        if (attr_header == nullptr) {
            attr_header = (ATTRIBUTE_HEADER*)((char*)&record_header + record_header.FirstAttributeOffset);
            return true;
        }

        // Move to the next header.
        attr_header = (ATTRIBUTE_HEADER*)((char*)attr_header + attr_header->RecordLength);

        uint record_end_offset = min(volume_data_.MFTRecordSize, (uint)record_header.RecLength);

        auto record_end = (char*)&record_header + record_end_offset;

        if (attr_header > (void*)record_end) return false;

        return attr_header->TypeCode != AttributeTypeCodes::END_INDICATOR;
    }

    int64 RawMFTRecordsParser::GetVariableLengthFieldValue(const char* field_begin, short field_size) {

        int64 res = 0;
        char* res_char = reinterpret_cast<char*>(&res);

        for (auto i = 0; i < field_size; ++i) {
            res_char[i] = field_begin[i];
        }

        if (field_begin[field_size - 1] < 0)  // negative char value
        {
            for (auto j = field_size; j < sizeof(int64); ++j) {
                res_char[j] = static_cast<char>(-1);
            }
        }

        return res;
    }

    uint RawMFTRecordsParser::GetFRN(const FILE_RECORD_HEADER& record_header) {
        return /*((uint64)record_header.SequenceNumber << 48) +*/ record_header.MFTRecordNumber;
    }

    bool RawMFTRecordsParser::RecordOK(FILE_RECORD_HEADER* record_header) const {

        bool record_ok = memcmp((void*)record_header->MultiSectorHeader.Signature, "FILE", 4) == 0 &&
                         (record_header->Flags & (int)FrhFlagsEnum::FRH_IN_USE) && Unfixup(record_header);

        return record_ok;
    }

    bool RawMFTRecordsParser::Unfixup(FILE_RECORD_HEADER* file_record_header) const {

        auto fixup_arr = (ushort*)((char*)file_record_header + file_record_header->MultiSectorHeader.FixupOffset);

        for (ushort i = 1; i < file_record_header->MultiSectorHeader.FixupSize; ++i) {
            // Last two bytes in each sector contain signature.

            ushort offset = i * volume_data_.BytesPerSector - sizeof(ushort);

            // The signature should appear the same in all sectors of the record.

            auto signature = (ushort*)((char*)(file_record_header) + offset);

            if (offset >= volume_data_.MFTRecordSize) break;

            if (fixup_arr[0] != *signature) return false;

            *signature = fixup_arr[i];
        }

        return true;
    }

    unique_ptr<vector<FileInfo*>> RawMFTRecordsParser::GetCompleteFileInfos() const {

        auto res = make_unique<vector<FileInfo*>>(accum_file_infos_->size());

        for (auto* accum_fi : *accum_file_infos_) {
            if (!accum_fi) continue;

            auto u_fi = accum_fi->GetFinalResult();
            if (!u_fi) continue;

            auto fi = u_fi.release();
            (*res)[fi->ID] = fi;
        }

        accum_file_infos_->clear();
        return res;
    }

    const char* RawMFTRecordsParser::GetResidentAttrOffset(const ATTRIBUTE_HEADER& attr_header) {

        const auto attr_offset = (const char*)&attr_header + attr_header.Form.Resident.ValueOffset;
        return attr_offset;
    }

    uint64 RawMFTRecordsParser::GetNonResidentDataAttrSize(const ATTRIBUTE_HEADER& attr_header) const {

        auto retrieval_pointers = ParseRunList(attr_header);

        uint64 file_size = 0;
        for (const auto& pair : *retrieval_pointers) {
            file_size += pair.second;
        }

        return file_size * volume_data_.BytesPerCluster;
    }

    void RawMFTRecordsParser::SetFileSizes(AccumulatedFileInfo* fi_info, const ATTRIBUTE_HEADER& attr_header) {

        if (attr_header.IsNotResident) {

            // The Real and Allocated Sizes are only present if the Starting VCN is zero.
            if (attr_header.Form.Nonresident.LowestVCN == 0) {
                if (fi_info->SizeReal && attr_header.NameLength)  // If size already set and this is ADS.
                    return;

                fi_info->SizeReal += attr_header.Form.Nonresident.FileSize;

                /*fi_info->SizeAllocated += attr_header.Form.Nonresident.CompressionUnitSize ?
                        attr_header.Form.Nonresident.TotalAllocated :
                        attr_header.Form.Nonresident.AllocatedLength;*/
            }
        } else {  // Resident.

            if (!attr_header.NameLength)  // Not an alternate data stream (ADS).
            {
                fi_info->SizeReal += attr_header.Form.Resident.ValueLength;
            }
        }
    }

} // namespace ntfs_reader