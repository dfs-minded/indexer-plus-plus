#pragma once

#include <fstream>
#include <memory>

#include "WindowsWrapper.h"

#include "Macros.h"

class RawMFTSerializer {
   public:
    NO_COPY(RawMFTSerializer)

    static RawMFTSerializer& Instance();

    void SerializeMFT(void* buffer, DWORD bytes_to_write) const;

    bool GetNtfsVolumeData(NTFS_VOLUME_DATA_BUFFER* volume_data_buff);

    bool ReadMFTFromFile(void* buffer, DWORD bytes_to_read);

   private:
    RawMFTSerializer();

    ~RawMFTSerializer();

    HANDLE raw_mft_file_;

    std::unique_ptr<std::ifstream> serialized_raw_mft_file_;
};
