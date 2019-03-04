// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <map>
#include <memory>

#include "Helpers.h"
#include "TextComparison.h"

namespace indexer_common {

	class Log;
    class FileInfo;

    typedef std::map<const char16_t*, int, WcharLessComparator> WcharToIntMap;
    typedef std::map<const char16_t*, const char16_t*, WcharLessComparator> WcharToWcharMap;


	// Facilitates work with FileInfo objects, provides additional FileInfo functions.

    class FileInfoHelper {
       public:
		static bool AllAscii(const char16_t* filename);

        static std::unique_ptr<const char16_t[]> GetPath(const FileInfo& fi, bool include_filename = false);


        // Returns the file extension (txt, rar, xml, etc.).

        static const char16_t* GetExtension(const FileInfo& fi);


        // Returns file MIME type (application, TXT file, shortcut etc.).

        static const char16_t* GetType(const FileInfo& fi);


        static const WcharToIntMap& GetDistinctOrderedExtensions(const std::vector<const FileInfo*>& files,
                                                                 std::vector<const char16_t*>* extensions);

        static const WcharToIntMap& GetDistinctOrderedTypes(const std::vector<const FileInfo*>& files,
                                                            std::vector<const char16_t*>* types);

       private:
        // A map form file extension to its MIME type. Used for caching.

        static WcharToWcharMap extension_to_type_map_;


        // A map form file extension to its index number (ordered int). Used for caching.

        static WcharToIntMap extension_to_int_map_;


        // A map form file type to its index number (ordered int). Used for caching.

        static WcharToIntMap type_to_int_map_;


        static const char16_t* kFileFolderTypename;


        static Log& Logger();
    };

    std::wostream& operator<<(std::wostream& os, const class FileInfo& FileInfo);


    bool operator==(const FileInfo& lhs, const FileInfo& rhs);


	// Serializes FileInfo for the test framework.

    std::wstring SerializeFileInfo(const FileInfo& fi);


	// Serializes FileInfo in human readable format.

    std::wstring SerializeFileInfoHumanReadable(const FileInfo& fi);


    std::unique_ptr<FileInfo> DeserializeFileInfo(const std::wstring& source);

} // namespace indexer_common