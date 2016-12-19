#pragma once

#ifdef WIN32
#include <Windows.h>
#include <io.h>
#else
#pragma pack(8)  //(push, 4)
#define FALSE 0
#define TRUE 1

#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_ARCHIVE 0x00000020
#define FILE_ATTRIBUTE_DEVICE 0x00000040
#define FILE_ATTRIBUTE_VIRTUAL 0x00010000

#define USN_REASON_DATA_OVERWRITE (0x00000001)
#define USN_REASON_DATA_EXTEND (0x00000002)
#define USN_REASON_DATA_TRUNCATION (0x00000004)
#define USN_REASON_NAMED_DATA_OVERWRITE (0x00000010)
#define USN_REASON_NAMED_DATA_EXTEND (0x00000020)
#define USN_REASON_NAMED_DATA_TRUNCATION (0x00000040)
#define USN_REASON_FILE_CREATE (0x00000100)
#define USN_REASON_FILE_DELETE (0x00000200)
#define USN_REASON_EA_CHANGE (0x00000400)
#define USN_REASON_SECURITY_CHANGE (0x00000800)
#define USN_REASON_RENAME_OLD_NAME (0x00001000)
#define USN_REASON_RENAME_NEW_NAME (0x00002000)
#define USN_REASON_INDEXABLE_CHANGE (0x00004000)
#define USN_REASON_BASIC_INFO_CHANGE (0x00008000)
#define USN_REASON_HARD_LINK_CHANGE (0x00010000)
#define USN_REASON_COMPRESSION_CHANGE (0x00020000)
#define USN_REASON_ENCRYPTION_CHANGE (0x00040000)
#define USN_REASON_OBJECT_ID_CHANGE (0x00080000)
#define USN_REASON_REPARSE_POINT_CHANGE (0x00100000)
#define USN_REASON_STREAM_CHANGE (0x00200000)
#define USN_REASON_TRANSACTED_CHANGE (0x00400000)
#define USN_REASON_INTEGRITY_CHANGE (0x00800000)
#define USN_REASON_CLOSE (0x80000000)

typedef void *HANDLE;

#define DUMMYSTRUCTNAME
typedef char CHAR;
typedef char BYTE;
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;
typedef unsigned int ULONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef ULONGLONG DWORDLONG;
typedef LONGLONG USN;
typedef char16_t WCHAR;
typedef WCHAR TCHAR;
typedef unsigned char UCHAR;
typedef const CHAR *LPCSTR, *PCSTR;
typedef void *LPVOID;
typedef DWORD *LPDWORD;

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;


typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef struct {
    DWORD RecordLength;
    WORD MajorVersion;
    WORD MinorVersion;
    DWORDLONG FileReferenceNumber;
    DWORDLONG ParentFileReferenceNumber;
    USN Usn;
    LARGE_INTEGER TimeStamp;
    DWORD Reason;
    DWORD SourceInfo;
    DWORD SecurityId;
    DWORD FileAttributes;
    WORD FileNameLength;
    WORD FileNameOffset;
    WCHAR FileName[1];

} USN_RECORD;

typedef struct {

    USN StartUsn;
    DWORD ReasonMask;
    DWORD ReturnOnlyOnClose;
    DWORDLONG Timeout;
    DWORDLONG BytesToWaitFor;
    DWORDLONG UsnJournalID;
    WORD MinMajorVersion;
    WORD MaxMajorVersion;

} READ_USN_JOURNAL_DATA, *PREAD_USN_JOURNAL_DATA;

typedef struct {

    DWORDLONG UsnJournalID;
    USN FirstUsn;
    USN NextUsn;
    USN LowestValidUsn;
    USN MaxUsn;
    DWORDLONG MaximumSize;
    DWORDLONG AllocationDelta;
    WORD MinSupportedMajorVersion;
    WORD MaxSupportedMajorVersion;

} USN_JOURNAL_DATA, *PUSN_JOURNAL_DATA;

// Date time
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;


typedef struct {
    LARGE_INTEGER VolumeSerialNumber;
    LARGE_INTEGER NumberSectors;
    LARGE_INTEGER TotalClusters;
    LARGE_INTEGER FreeClusters;
    LARGE_INTEGER TotalReserved;
    DWORD BytesPerSector;
    DWORD BytesPerCluster;
    DWORD BytesPerFileRecordSegment;
    DWORD ClustersPerFileRecordSegment;
    LARGE_INTEGER MftValidDataLength;
    LARGE_INTEGER MftStartLcn;
    LARGE_INTEGER Mft2StartLcn;
    LARGE_INTEGER MftZoneStart;
    LARGE_INTEGER MftZoneEnd;

} NTFS_VOLUME_DATA_BUFFER, *PNTFS_VOLUME_DATA_BUFFER;

typedef struct {

    DWORDLONG StartFileReferenceNumber;
    USN LowUsn;
    USN HighUsn;
    WORD MinMajorVersion;
    WORD MaxMajorVersion;

} MFT_ENUM_DATA, *PMFT_ENUM_DATA;

bool FileTimeToLocalFileTime(const FILETIME *lpFileTime, LPFILETIME lpLocalFileTime);
void GetLocalTime(LPSYSTEMTIME lpSystemTime);
bool SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, LPFILETIME lpFileTime);
bool FileTimeToSystemTime(const FILETIME *lpFileTime, LPSYSTEMTIME lpSystemTime);
ULONGLONG GetTickCount64(void);
DWORD GetTickCount();
void Sleep(DWORD dwMilliseconds);
//#pragma pack(pop)
#endif
