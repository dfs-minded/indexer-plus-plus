// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "MockMFTReader.h"

#include "CommandlineArguments.h"
#include "FileInfo.h"

#include "FileInfoObjectsSerializer.h"

std::unique_ptr<MFTReadResult> MockMFTReader::ReadAllRecords() {
    auto res = std::make_unique<MFTReadResult>();

    auto filename = CommandlineArguments::Instance().ReplayFileInfosPath;

    auto u_deserialized = FileInfoObjectsSerializer::Instance().DeserializeAllFileInfos(filename);
    res->Data.swap(u_deserialized);

    // Find and set the root.
    for (auto* fi : *res->Data) {
        if (fi->ID == fi->ParentID) {
            res->Root = fi;
            break;
        }
    }

    return res;
}
