#pragma once

#include <memory>

#include "FileInfo.h"

class FileInfoObjectsSerializer {
   public:
    NO_COPY(FileInfoObjectsSerializer)

    static FileInfoObjectsSerializer& Instance();

    void SerializeFileInfoToFile(const FileInfo& fi) const;

    std::unique_ptr<std::vector<FileInfo*>> DeserializeAllFileInfos(const std::wstring& filename) const;

   private:
    FileInfoObjectsSerializer();

    ~FileInfoObjectsSerializer();

    FILE* file_infos_db_;
};
