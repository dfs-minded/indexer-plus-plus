// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#include "WindowsWrapper.h"

#include "Helper.h"

using namespace std;

class USNRecordSerializationTest : public testing::Test {

   public:
    void SetUp() override {

        // This record was created from the real data.
        // Malloc is needed to hold record in one memory block, not to split it. This made for the convenience to put
        // everything in one buffer (not to have separate buffer for other memory piece for FileName property).

        auto record_size = 104;
        record_          = static_cast<USN_RECORD*>(malloc(record_size));

        record_->RecordLength              = record_size;
        record_->MajorVersion              = 2;
        record_->MinorVersion              = 0;
        record_->FileReferenceNumber       = 2251799813685291;
        record_->ParentFileReferenceNumber = 1407374883553285;
        record_->Usn                       = 41224;

        record_->TimeStamp.HighPart = 30515100;
        record_->TimeStamp.LowPart  = 2997946383;

        record_->Reason         = 256;
        record_->SourceInfo     = 0;
        record_->SecurityId     = 0;
        record_->FileAttributes = 32;
        record_->FileNameLength = 42;
        record_->FileNameOffset = 60;

        memcpy(record_->FileName, L"New Text Document.txt", record_->FileNameLength);
    }

    void TearDown() override {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be

        free(record_);
    }

    // put in any custom data members that you need
    USN_RECORD* record_;
    char drive_letter_ = 'X';
};

// Test USN record serialization and deserialization, namely that deserialized record equal to original. 

TEST_F(USNRecordSerializationTest, DeserializedRecordEqToOrig) {

	const wstring serialized = Helper::SerializeRecord(*record_, drive_letter_);

	auto deserializedPair = Helper::DeserializeRecord(serialized);

	EXPECT_TRUE(deserializedPair.first != nullptr);
	EXPECT_EQ(deserializedPair.second, drive_letter_);

	auto record = move(deserializedPair.first);

	EXPECT_EQ(record_->RecordLength,				record->RecordLength);
	EXPECT_EQ(record_->MajorVersion,				record->MajorVersion);
	EXPECT_EQ(record_->MinorVersion,				record->MinorVersion);
	EXPECT_EQ(record_->FileReferenceNumber,			record->FileReferenceNumber);
	EXPECT_EQ(record_->ParentFileReferenceNumber,	record->ParentFileReferenceNumber);
	EXPECT_EQ(record_->Usn, record->Usn);

	EXPECT_EQ(record_->TimeStamp.LowPart,			record->TimeStamp.LowPart);
	EXPECT_EQ(record_->TimeStamp.HighPart,			record->TimeStamp.HighPart);

	EXPECT_EQ(record_->Reason,						record->Reason);
	EXPECT_EQ(record_->SourceInfo,					record->SourceInfo);
	EXPECT_EQ(record_->SecurityId,					record->SecurityId);

	EXPECT_EQ(record_->FileAttributes,				record->FileAttributes);
	EXPECT_EQ(record_->FileNameLength,				record->FileNameLength);
	EXPECT_EQ(record_->FileNameOffset,				record->FileNameOffset);

	EXPECT_TRUE(0 == memcmp(record_->FileName, record->FileName, record_->FileNameLength));
}
