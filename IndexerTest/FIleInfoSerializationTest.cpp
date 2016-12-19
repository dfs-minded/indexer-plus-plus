// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#include "FileInfo.h"
#include "FileInfoHelper.h"
#include "Helpers.h"

TEST(FIleInfoSerializationTest, DeserializedFileInfoEqToOrig) {

    FileInfo fi('X');
    fi.ID       = 123455678;
    fi.ParentID = 908765432;

    fi.CopyAndSetName(__L__("lemon ~ tree"), 10);
    fi.FileAttributes = 26;

    fi.SizeReal = 1020;
    // fi.SizeAllocated = 1024;

    fi.CreationTime   = 134033334;
    fi.LastAccessTime = 135022224;
    fi.LastWriteTime  = 136066664;

    auto serialized = SerializeFileInfo(fi);
    auto deserialized = DeserializeFileInfo(serialized);

    EXPECT_TRUE(fi == *deserialized.get());
}