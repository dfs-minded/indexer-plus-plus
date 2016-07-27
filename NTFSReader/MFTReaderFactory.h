#pragma once

#include "Macros.h"

#include "MFTReader.h"

class MFTReaderFactory {
   public:
    NO_COPY(MFTReaderFactory)

    static std::unique_ptr<MFTReader> CreateReader(char drive_letter);
};
