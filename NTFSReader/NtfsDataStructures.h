// This file was written using sources listed below (which are not exhaustive and are not limited to),
// as well as the result of my own work and research.
//
// https://msdn.microsoft.com/en-us/library/bb470206(v=vs.85).aspx
//
// http://grayscale-research.org/new/pdfs/NTFS%20forensics.pdf
//
// https://flatcap.org/linux-ntfs/ntfs/
//
// File System Forensic Analysis
// Author(s) :Carrier, Brian
// Publisher : Addison - Wesley Professional PTG
// Print ISBN : n9780321268174, 0321268172
// eText ISBN : 9780134439549
// https://blogs.technet.microsoft.com/joscon/2011/01/06/how-hard-links-work/
//
// https://msdn.microsoft.com/en-us/library/bb470211(v=vs.85).aspx
// Represents an address in the master file table (MFT). The address is tagged with a circularly reused sequence number
// that is set at the time the MFT segment reference was valid.

#pragma once

#include "WindowsWrapper.h"

#pragma pack(push, 4)

namespace ntfs_reader {

    typedef struct _MFT_SEGMENT_REFERENCE {
        // First 48 bit are MFT record sequential number.
        ULONG SegmentNumberLowPart;
        USHORT SegmentNumberHighPart;

        // The nonzero sequence number (MFT record allocations number).
        USHORT SequenceNumber;

    } MFT_SEGMENT_REFERENCE, *P_MFT_SEGMENT_REFERENCE;


// https://msdn.microsoft.com/en-us/library/bb470211(v=vs.85).aspx
//  A file reference in NTFS is simply the MFT Segment Reference of the Base file record.
    typedef MFT_SEGMENT_REFERENCE FILE_REFERENCE, *P_FILE_REFERENCE;

// https://msdn.microsoft.com/en-us/library/bb470212(v=vs.85).aspx
// Represents the multisector header.
    typedef struct _MULTI_SECTOR_HEADER {
        UCHAR Signature[4];  // Normal entries start with the "FILE" signature.
        USHORT FixupOffset;
        USHORT FixupSize;

    } MULTI_SECTOR_HEADER, *P_MULTI_SECTOR_HEADER;

// https://blogs.technet.microsoft.com/joscon/2011/01/06/how-hard-links-work/
// http://www.cse.scu.edu/~tschwarz/coen252_07Fall/Lectures/NTFS.html
// Represents the file record segment. This is the header for each file record in MFT.
    typedef struct _FILE_RECORD_SEGMENT_HEADER {
        MULTI_SECTOR_HEADER MultiSectorHeader;
        ULONGLONG LogSeqNumber;
        USHORT SequenceNumber;
        USHORT HardLinks;
        USHORT FirstAttributeOffset;
        USHORT Flags;
        ULONG RecLength;
        ULONG AllocatedLength;

        // A file reference to the base file record segment for this file. If this is the base file record, the value is 0.
        FILE_REFERENCE BaseFileRecordSegment;
        USHORT NextAttrID;
        USHORT AlignTo4ByteBoundary;
        ULONG MFTRecordNumber;
    } FILE_RECORD_HEADER, *P_FILE_RECORD_HEADER;


// FILE_RECORD_HEADER Flags attribute possible values.
    enum class FrhFlagsEnum {
        FRH_IN_USE = 0x0001,  // Record is in use.
        FRH_DIRECTORY = 0x0002,  // Record is a directory.
    };


    enum class AttributeTypeCodes {
        STANDARD_INFORMATION = 0x10,
        ATTRIBUTE_LIST = 0x20,
        FILENAME = 0x30,
        OBJECT_ID = 0x40,
        SECURITY_DESCRIPTOR = 0x50,
        VOLUME_NAME = 0x60,
        VOLUME_INFORMATION = 0x70,
        DATA = 0x80,
        INDEX_ROOT = 0x90,
        INDEX_ALLOCATION = 0xA0,
        BITMAP = 0xB0,
        REPARSE_POINT = 0xC0,
        EA_INFORMATION = 0xD0,
        EA = 0xE0,
        END_INDICATOR = -1
    };

// An MFT entry consists of attributes, and each attribute has the same header data structure.
    typedef struct _ATTRIBUTE_RECORD_HEADER {
        AttributeTypeCodes TypeCode;
        ULONG RecordLength;
        UCHAR IsNotResident;  // 1 if not a resident, 0 - if a resident.
        UCHAR NameLength;
        USHORT NameOffset;
        USHORT Flags;
        USHORT Instance;
        union {
            struct RESIDENT {
                // Size of the content.
                ULONG ValueLength;

                // Offset to the content (relative to the start of the attribute).
                USHORT ValueOffset;

                UCHAR Reserved[2];
            } Resident;

            struct NON_RESIDENT {
                ULONGLONG LowestVCN;
                ULONGLONG HighestVCN;
                USHORT DatarunOffset;
                UCHAR CompressionUnitSize;
                UCHAR Reserved[5];
                LONGLONG AllocatedLength;
                LONGLONG FileSize;
                LONGLONG ValidDataLength;
                LONGLONG TotalAllocated;
            } Nonresident;
        } Form;
    } ATTRIBUTE_HEADER, *P_ATTRIBUTE_HEADER;


// The attribute exists for all files and directories and contains the core metadata. It is always a resident.
    typedef struct _STANDARD_INFORMATION {
        LONGLONG CreationTime;

        // The time that the content of the $DATA or $INDEX attributes was last modified.
        LONGLONG LastModificationTime;

        // The time that the metadata of the file was last modified (not shown in Windows).
        LONGLONG LastChangeTime;

        // The time that the content of the file was last accessed.
        LONGLONG LastAccessTime;

        // Flags, also called DOS File Permissions (Read only, system, archive, compressed, sparse, or encrypted).
        // This value will not identify if an entry is for a file or a directory, see flags in the MFT entry header.
        ULONG FileAttributes;

        ULONG MaxVersionsNum;
        ULONG VersionNum;
        ULONG ClassID;
        ULONG OwnerID;
        ULONG SecurityID;
        LONGLONG Quota;

        // Last Update Sequence Number that was created in USN Journal for this file. If zero, the USN Journal is disabled.
        LONGLONG USN;

    } STANDARD_INFORMATION, *P_STANDARD_INFORMATION;


// Duplicated in STANDARD_INFORMATION and FILE_NAME_INFORMATION attributes information.
    typedef struct _DUPLICATED_INFORMATION {
        LONGLONG CreationTime;
        LONGLONG LastModificationTime;
        LONGLONG LastChangeTime;
        LONGLONG LastAccessTime;

        LONGLONG AllocatedFileSize;
        LONGLONG FileSize;

        ULONG FileAttributes;

    } DUPLICATED_INFORMATION, *P_DUPLICATED_INFORMATION;


// Every file and directory has at least one $FILE_NAME attribute in its MFT entry.
    typedef struct _FILE_NAME_INFORMATION {
        FILE_REFERENCE /*ULONGLONG*/ ParentDirectory;

        DUPLICATED_INFORMATION DuplicatedInfo;

        USHORT PackedEaSize;
        USHORT Reserved;

        UCHAR FilenameLength;

        // FILE_NAME flags (readonly, archive, hidden etc. see below for more description).
        UCHAR Flags;

        // Pointer to the beginning of the filename.
        WCHAR Filename[1];

    } FILE_NAME_INFORMATION, *P_FILE_NAME_INFORMATION;


    enum class FilenameFlagsEnum {
        EMPTY = -1,
        POSIX = 0,
        WIN_32 = 1,
        DOS = 2,
        WIN_32_AND_DOS = 3,

        // This bit is duplicated from the file record, to indicate that this file has a file name index present
        // (namely is a "directory").
        NTFS_FILE_ATTRIBUTE_DIRECTORY = 0x10000000
    };

// More detailed file attributes description on MSDN:
// https://msdn.microsoft.com/en-us/library/gg258117(v=vs.85).aspx
// or Linux NTFS doc:
// https://flatcap.org/linux-ntfs/ntfs/attributes/file_name.html

#pragma pack(pop)
} // namespace ntfs_reader