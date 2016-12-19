// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Macros.h"
#include "typedefs.h"

#include "AccumulatedFileInfo.h"
#include "NTFSDataStructures.h"
#include "VolumeData.h"

// Parses raw MFT records and creates corresponding FileInfo objects.

class RawMFTRecordsParser {
   public:
    explicit RawMFTRecordsParser(VolumeData volume_data);

    NO_COPY(RawMFTRecordsParser)


    // Parses raw MFT entries from the buffer. For each entry creates and fills with parsed data FileInfoBuilder object.
    // |recordsNum| - the number of records in |buff| to parse.

    void Parse(const char* buffer, uint records_num) const;


    // For fragmented files we need to parse run list in order to get file clusters location.
    // Returns: pair<run_offset, run_length> - data location of the MFT file.

    std::unique_ptr<std::vector<std::pair<int64, int64>>> GetMFTRetrievalPointers(char* buff) const;


    // Returns the list of files, which data was accumulated during MFT parsing and have all its properties set.
    // This FileInfos are mainly created due to the child records, which appeared in the MFT before their base records,
    // and we needed to wait for the base record to complete gathering information about particular FileInfo object.
    // Some of the FileInfos will still incomplete, and this function call will also delete them.
    // Some of the FileInfos in the returned vector will be nullptr, since not for all indices in the allocated with the
    // MFT size vector existed corresponding MFT record.

    std::unique_ptr<std::vector<FileInfo*>> GetCompleteFileInfos() const;

    mutable int files_with_hardlinks_num_ = 0;
    mutable int not_in_use_for_debug_ = 0;

   private:
    // Iterates throw MFT record header attributes and writes their valuable content such as file size,
    // timestamps, parent ID, etc. into |fi_info| object.

    void ParseRecordAttributes(const FILE_RECORD_HEADER& file_record_header, AccumulatedFileInfo* fi_info) const;


    static bool IsDirectory(const FILE_RECORD_HEADER& file_record_header);


    // Reads object value from memory, which starts at |field_begin| and has size |field_size| bytes.

    static int64 GetVariableLengthFieldValue(const char* field_begin, short field_size);


    // For NonResident Data attribute gets attribute size, considering that non-resident attributes have
    // runlist, which describes attribute location on the disk (so it could be separated in several parts
    // and each run in the runlist correspond to one part).

    uint64 GetNonResidentDataAttrSize(const ATTRIBUTE_HEADER& attr_header) const;


    // Returns a data structure (vector of <run_offset, run_length>) that describes the allocation and location
    // on disk of a specific attribute.

    std::unique_ptr<std::vector<std::pair<int64, int64>>> ParseRunList(const ATTRIBUTE_HEADER& attr_header) const;


    // Gets the attribute offset related to the beginning of the attribute header.

    static const char* GetResidentAttrOffset(const ATTRIBUTE_HEADER& attr_header);


    // Sets the |attr_header| to the next attribute header in the record. Returns true if
    // the header is valid and was set, false otherwise.
    // |record_header| is the current MFT entry header being processed.

    bool GetNextAttrHeader(const FILE_RECORD_HEADER& record_header, P_ATTRIBUTE_HEADER& attr_header) const;


    // Returns the unique file reference number by its record header.

    static uint GetFRN(const FILE_RECORD_HEADER& record_header);


    // Checks if the record |record_header| has "FILE" signature, if it is in use,
    // checks and removes fixup values int the record.

    bool RecordOK(FILE_RECORD_HEADER* record_header) const;


    // Unfixup function replaces the signature values by their original content and checks for damaged sectors and
    // corrupted data structures. Returns true if record is not corrupted and false otherwise.

    bool Unfixup(FILE_RECORD_HEADER* file_record_header) const;


    static void SetFileSizes(AccumulatedFileInfo* builder, const ATTRIBUTE_HEADER& attr_header);


    VolumeData volume_data_;


    // A vector of objects, that accumulate parsed MFT records data into the internal FileInfo object.
    // Array index corresponds to the ID.

    std::unique_ptr<std::vector<AccumulatedFileInfo*>> accum_file_infos_;
};
