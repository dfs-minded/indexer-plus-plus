#pragma once

#include <string>
#include "WindowsWrapper.h"

#include "Macros.h"
#include "typedefs.h"

#include "NTFSDataStructures.h"

class Serializer {
   public:
    Serializer();

    NO_COPY(Serializer)

    ~Serializer();

    const Serializer* Serialize(P_FILE_RECORD_HEADER record_header);

    const Serializer* Serialize(const std::wstring& data);

    const Serializer* Serialize(uint64 data);

    const Serializer* Endl();

    const Serializer* Delim();

   private:
    void WriteToFile(const std::wstring& data) const;

    FILE* mft_serialization_file_;

    static const std::wstring kDelim;
};
