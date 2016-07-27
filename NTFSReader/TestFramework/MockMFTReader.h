#pragma once

#include <memory>

#include "Macros.h"

#include "MFTReadResult.h"
#include "MFTReader.h"

class MockMFTReader : public MFTReader {
   public:
    NO_COPY(MockMFTReader)

    MockMFTReader() = default;

    virtual std::unique_ptr<MFTReadResult> ReadAllRecords() override;
};
