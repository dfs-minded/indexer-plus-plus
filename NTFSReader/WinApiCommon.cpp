#include "WinApiCommon.h"

#include "WindowsWrapper.h"

#include "FileInfo.h"
#include "HelperCommon.h"
#include "IndexerDateTime.h"
#include "Log.h"

using namespace std;

HANDLE WinApiCommon::OpenVolume(char drive_letter) {

    wchar_t pattern[10] = L"\\\\?\\a:";

    pattern[4] = static_cast<wchar_t>(drive_letter);
    HANDLE volume = nullptr;
#ifdef WIN32
    volume = CreateFile(
        pattern,  // lpFileName
        // also could be | FILE_READ_DATA | FILE_READ_ATTRIBUTES | SYNCHRONIZE
        GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE,              // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,  // share mode
        NULL,                                                    // default security attributes
        OPEN_EXISTING,                                           // disposition
        // It is always set, no matter whether you explicitly specify it or not. This means, that access
        // must be aligned with sector size so we can only read a number of bytes that is a multiple of the sector size.
        FILE_FLAG_NO_BUFFERING,  // file attributes
        NULL                     // do not copy file attributes
        );
    if (volume == INVALID_HANDLE_VALUE) {
        WriteToOutput(METHOD_METADATA + HelperCommon::GetLastErrorString());
    }
#endif
    return volume;
}

HANDLE WinApiCommon::CreateFileForWrite(const wstring& filename) {
    HANDLE h_file = nullptr;
#ifdef WIN32
    h_file = CreateFile(filename.c_str(),       // name of the write file
                        GENERIC_WRITE,          // open for writing
                        0,                      // do not share
                        NULL,                   // default security
                        CREATE_NEW,             // create new file only
                        FILE_ATTRIBUTE_NORMAL,  // normal file
                        NULL);                  // no attr. template
#endif
    return h_file;
}

HANDLE WinApiCommon::OpenFileForRead(const wstring& filename) {
    HANDLE h_file = nullptr;
#ifdef WIN32
    h_file = CreateFile(filename.c_str(),       // file to open
                        GENERIC_READ,           // open for reading
                        FILE_SHARE_READ,        // share for reading
                        NULL,                   // default security
                        OPEN_EXISTING,          // existing file only
                        FILE_ATTRIBUTE_NORMAL,  // normal file
                        NULL);                  // no attr. template

    if (h_file == INVALID_HANDLE_VALUE) {
        WriteToOutput(METHOD_METADATA + HelperCommon::GetLastErrorString());
    }
#endif
    return h_file;
}

bool WinApiCommon::ReadBytes(HANDLE h_file, LPVOID buffer, DWORD bytes_to_read) {
    DWORD read = 0;
#ifdef WIN32
    bool res = ReadFile(h_file,         // file handle
                        buffer,         // lpBuffer
                        bytes_to_read,  // nNumberOfBytesToRead
                        &read,          // lpNumberOfBytesRead
                        NULL);          // lpOverlapped

    if (!res || bytes_to_read != read) {
        WriteToOutput(METHOD_METADATA + HelperCommon::GetLastErrorString());
    }
    return res;
#endif
    return true;
}

void WinApiCommon::WriteBytes(HANDLE h_file, LPVOID buffer, DWORD bytes_to_write) {

    DWORD num_of_bytes_written;
#ifdef WIN32
    bool ok = WriteFile(h_file,                 // open file handle
                        buffer,                 // start of data to write
                        bytes_to_write,         // number of bytes to write
                        &num_of_bytes_written,  // number of bytes that were written
                        NULL);                  // no overlapped structure

    if (!ok || bytes_to_write != num_of_bytes_written) {
        WriteToOutput(METHOD_METADATA + HelperCommon::GetLastErrorString());
    }
#endif
}

void WinApiCommon::SetFilePointerFromFileBegin(HANDLE volume, ULONGLONG bytes_to_move) {

    long low_part  = (bytes_to_move << 32) >> 32;
    long high_part = bytes_to_move >> 32;
#ifdef WIN32
    auto current_file_pos = SetFilePointer(volume, low_part, &high_part, FILE_BEGIN);
#endif
}

bool WinApiCommon::GetNtfsVolumeData(HANDLE h_file, NTFS_VOLUME_DATA_BUFFER* volume_data_buff) {

    DWORD read;
    DWORD volume_data_size = sizeof(*volume_data_buff);
#ifdef WIN32
    bool ok =
        DeviceIoControl(h_file, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0, volume_data_buff, volume_data_size, &read, NULL);

    return ok;
#endif
    return true;
}

bool WinApiCommon::LoadJournal(HANDLE volume, USN_JOURNAL_DATA* journal_data) {

    DWORD byte_count;
#ifdef WIN32
    // Try to open journal.
    if (!DeviceIoControl(volume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, journal_data, sizeof(*journal_data), &byte_count,
                         NULL)) {

        // If failed (for example, in case journal is disabled), create journal and retry.
        WriteToOutput(METHOD_METADATA + HelperCommon::GetLastErrorString());

        if (CreateJournal(volume)) {
            return LoadJournal(volume, journal_data);
        }

        return false;
    }
#endif
    return true;
}

bool WinApiCommon::CreateJournal(HANDLE volume) {
#ifdef WIN32
    DWORD byte_count;
    CREATE_USN_JOURNAL_DATA create_journal_data;

    bool ok = DeviceIoControl(volume,                       // handle to volume
                              FSCTL_CREATE_USN_JOURNAL,     // dwIoControlCode
                              &create_journal_data,         // input buffer
                              sizeof(create_journal_data),  // size of input buffer
                              NULL,                         // lpOutBuffer
                              0,                            // nOutBufferSize
                              &byte_count,                  // number of bytes returned
                              NULL) != 0;                   // OVERLAPPED structure

    if (!ok) {
        WriteToOutput(METHOD_METADATA + HelperCommon::GetLastErrorString());
    }

    return ok;
#else
    return true;
#endif
}

bool WinApiCommon::GetSizeAndTimestamps(const wchar_t& path, FileInfo* file_info) {
#ifdef WIN32
    WIN32_FILE_ATTRIBUTE_DATA file_attr_data;
    bool ok = GetFileAttributesEx(&path, GetFileExInfoStandard, &file_attr_data);
    if (!ok) return false;

    if (!file_info->IsDirectory())  // Using sizes only for files.
    {
        file_info->SizeReal = HelperCommon::SizeFromBytesToKiloBytes(
            HelperCommon::PairDwordToInt64(file_attr_data.nFileSizeHigh, file_attr_data.nFileSizeLow));
    }

    file_info->CreationTime   = IndexerDateTime::FiletimeToUnixTime(file_attr_data.ftCreationTime);
    file_info->LastAccessTime = IndexerDateTime::FiletimeToUnixTime(file_attr_data.ftLastAccessTime);
    file_info->LastWriteTime  = IndexerDateTime::FiletimeToUnixTime(file_attr_data.ftLastWriteTime);
#endif
    return true;
}