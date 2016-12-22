// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "FileInfoObjectsSerializer.h"

#include <fcntl.h>
#include <string>

#include "FileInfoHelper.h"
#include "HelperCommon.h"
#include "IndexerDateTime.h"
#include "WindowsWrapper.h"

namespace ntfs_reader {

    using namespace std;

    FileInfoObjectsSerializer& FileInfoObjectsSerializer::Instance() {
        static FileInfoObjectsSerializer instance_;
        return instance_;
    }

    FileInfoObjectsSerializer::FileInfoObjectsSerializer() {
        auto filename = "FileInfosDB_" + to_string(IndexerDateTime::TicksNow()) + ".txt";

        file_infos_db_ = fopen(filename.c_str(), "w");
#ifdef WIN32
        _setmode(_fileno(file_infos_db_), _O_U8TEXT);
#endif
    }

    FileInfoObjectsSerializer::~FileInfoObjectsSerializer() {
        fclose(file_infos_db_);
    }

    void FileInfoObjectsSerializer::SerializeFileInfoToFile(const FileInfo& fi) const {
        fwprintf(file_infos_db_, L"%s\n", SerializeFileInfo(fi).c_str());
        fflush(file_infos_db_);
    }

    unique_ptr<vector<FileInfo*>> FileInfoObjectsSerializer::DeserializeAllFileInfos(const wstring& filename) const {
#ifdef WIN32
        FILE* file_infos_in = _wfopen(filename.c_str(), L"r");
        _setmode(_fileno(file_infos_in), _O_U8TEXT);
#else
        FILE* file_infos_in = fopen(HelperCommon::WStringToString(filename).c_str(), "r");
#endif

        wchar_t buffer[1001];

        // Starting from size 1, but will double quickly as the number of parsed records increases.
        auto data = make_unique<vector<FileInfo*>>(1);

        while (fgetws(buffer, 1000, file_infos_in)) {
            FileInfo* fi = DeserializeFileInfo(buffer).release();
            if (data->size() <= fi->ID) data->resize(data->size() * 2);
            (*data)[fi->ID] = fi;
        }

        fclose(file_infos_in);

        return data;
    }

} // namespace ntfs_reader