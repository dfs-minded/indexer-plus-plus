// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "FileInfoHelper.h"

#include <string.h>

#include "AsyncLog.h"
#include "EmptyLog.h"
#include "OneThreadLog.h"
#include "FileInfo.h"
#include "Helpers.h"

namespace indexer_common {

    using namespace std;

	bool FileInfoHelper::AllAscii(const char16_t* filename)
	{
		char16_t c = *filename;
		while (c != '\0') {
			if (c > 127) return false;
			++filename;
			c = *filename;
		}
		return true;
	}

	unique_ptr<const char16_t[]> FileInfoHelper::GetPath(const FileInfo& fi, bool include_filename) {

        vector<const FileInfo*> path_to_root;
        path_to_root.reserve(15);

        const FileInfo* current = &fi;

        if (!include_filename) current = current->Parent;

        ushort total_name_length = 0;

        while (true) {
            path_to_root.push_back(current);
            total_name_length += current->NameLength + 1;  // For path separators and '\0' in the path end.
            if (current->ID == current->ParentID) break;
            current = current->Parent;
        }

        auto res = make_unique<const char16_t[]>(total_name_length);
        auto curr = const_cast<char16_t*>(res.get());

        for (int i = path_to_root.size() - 1; i >= 0; --i) {
            if (curr != res.get())  // Not the drive name.
            {
                *curr = '\\';
                ++curr;
            }

            auto num_bytes = path_to_root[i]->NameLength << 1;
            memcpy(curr, (void*)path_to_root[i]->GetName(), num_bytes);
            curr += (num_bytes >> 1);
        }

        *curr = '\0';
        return res;
    }

    const char16_t* FileInfoHelper::GetExtension(const FileInfo& fi) {
        if (fi.IsDirectory() || fi.NameLength == 0) return Empty16String;
        auto* name = fi.GetName();

        for (auto c = name + fi.NameLength - 1; c != name; --c)
            if (*c == '.') return c;
        return Empty16String;
    }

    WcharToWcharMap FileInfoHelper::extension_to_type_map_;

    const char16_t* FileInfoHelper::kFileFolderTypename =
        reinterpret_cast<const char16_t*>(L"File folder");  // TODO add localization.

    Log& FileInfoHelper::Logger() {
        static Log* logger_;
        if (!logger_) {
            GET_LOGGER
        }
        return *logger_;
    }

    const char16_t* FileInfoHelper::GetType(const FileInfo& fi) {

#ifdef WIN32
        if (fi.IsDirectory()) return kFileFolderTypename;

        const auto* ext = GetExtension(fi);
        const auto& it_res = extension_to_type_map_.find(ext);

        if (it_res != extension_to_type_map_.end()) {
            return it_res->second;
        }

        auto u_full_name = GetPath(fi, true);

        SHFILEINFO info{};
        auto file_attributes = FILE_ATTRIBUTE_NORMAL;
        auto flags = SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES;

        bool ok = SHGetFileInfo(reinterpret_cast<const wchar_t*>(u_full_name.get()), 
								file_attributes,
								&info,
								sizeof(info),
								flags);

        if (!ok) Logger().Error(METHOD_METADATA + L"SHGetFileInfo failed.");

        auto u_type_name = helpers::CopyToNewWchar(reinterpret_cast<char16_t*>(info.szTypeName));
        auto u_ext = helpers::CopyToNewWchar(ext);

        extension_to_type_map_[u_ext.release()] = u_type_name.release();
        return extension_to_type_map_[ext];
#else
        return u"Type";
#endif
    }


    WcharToIntMap FileInfoHelper::extension_to_int_map_;

    WcharToIntMap FileInfoHelper::type_to_int_map_;

    const WcharToIntMap& FileInfoHelper::GetDistinctOrderedExtensions(const vector<const FileInfo*>& files,
                                                                      vector<const char16_t*>* extensions) {
        Logger().Debug(METHOD_METADATA + L" Current ext count = " + to_wstring(extension_to_int_map_.size()));

        extensions->reserve(files.size());
        bool need_reindex = false;  // Indicates whether extension ordered number needed recalculation.

        for (size_t i = 0; i < files.size(); ++i) {
            const auto* ext = GetExtension(*files[i]);
            extensions->push_back(ext);
            if (extension_to_int_map_.find(ext) == extension_to_int_map_.end()) {
                need_reindex = true;
                auto new_ext = helpers::CopyToNewWchar(ext);
                extension_to_int_map_.emplace(new_ext.release(), 0);
            }
        }

        if (need_reindex) {
            auto i = 0;
            for (auto& key_val : extension_to_int_map_) {
                key_val.second = i++;
            }
        }

        return extension_to_int_map_;
    }

    const WcharToIntMap& FileInfoHelper::GetDistinctOrderedTypes(const vector<const FileInfo*>& files,
                                                                 vector<const char16_t*>* types) {
        Logger().Debug(METHOD_METADATA + L" Current types count = " + to_wstring(type_to_int_map_.size()));

        types->reserve(files.size());
        bool need_reindex = false;  // Indicates whether types ordered number needed recalculation.

        for (size_t i = 0; i < files.size(); ++i) {
            const auto* type = GetType(*files[i]);
            types->push_back(type);
            if (type_to_int_map_.find(type) == type_to_int_map_.end()) {
                need_reindex = true;
                type_to_int_map_.emplace(type, 0);
            }
        }

        if (need_reindex) {
            auto i = 1;
            for (auto& key_val : type_to_int_map_) {
                key_val.second = i++;
            }

            // Folders must be listed first in sorting.
            type_to_int_map_[kFileFolderTypename] = 0;
        }

        return type_to_int_map_;
    }


    wostream& operator<<(wostream& os, const class FileInfo& fi) {
        auto u_full_name = FileInfoHelper::GetPath(fi, true);
        os << fi.ID << L" " << u_full_name.get();
        return os;
    }

    const wstring g_delim = L"|";

    bool operator==(const FileInfo& lhs, const FileInfo& rhs) {
        if (&lhs == &rhs) return true;

        bool res = lhs.DriveLetter == rhs.DriveLetter && lhs.ID == rhs.ID && lhs.ParentID == rhs.ParentID &&
                   lhs.NameLength == rhs.NameLength && memcmp(lhs.GetName(), rhs.GetName(), lhs.NameLength) == 0 &&
                   lhs.SizeReal == rhs.SizeReal &&
                   /*lhs.SizeAllocated == rhs.SizeAllocated &&*/
                   lhs.CreationTime == rhs.CreationTime && lhs.LastAccessTime == rhs.LastAccessTime &&
                   lhs.LastWriteTime == rhs.LastWriteTime;

        return res;
    }

    wstring SerializeFileInfo(const FileInfo& fi) {
        wstring res;

        res += fi.DriveLetter;  // 0
        res += g_delim;
        res += to_wstring(fi.ID) + g_delim;          // 1
        res += to_wstring(fi.ParentID) + g_delim;    // 2
        res += to_wstring(fi.NameLength) + g_delim;  // 3

        res += helpers::Char16ToWstring(fi.GetName()) + g_delim;  // 4

        res += to_wstring(fi.FileAttributes) + g_delim;  // 5

        res += to_wstring(fi.SizeReal) + g_delim;  // 6
        // res += to_wstring(fi.SizeAllocated) + g_delim; //

        res += to_wstring(fi.CreationTime) + g_delim;    // 7
        res += to_wstring(fi.LastAccessTime) + g_delim;  // 8
        res += to_wstring(fi.LastWriteTime) + g_delim;   // 9

        return res;
    }

    wstring SerializeFileInfoHumanReadable(const FileInfo& fi) {
        wstring res;

        res += L"; ID = " + to_wstring(fi.ID);
        res += L"; ParentID = " + to_wstring(fi.ParentID);
        res += L"; FileInfo: Name = " + helpers::Char16ToWstring(fi.GetName());
        res += L"; NameLength = " + to_wstring(fi.NameLength);
        res += L"; FileAttributes = " + to_wstring(fi.FileAttributes);

        res += L"; SizeReal = " + to_wstring(fi.SizeReal);
        // res += L"; SizeAllocated = " + to_wstring(fi.SizeAllocated);

        res += L"; CreationTime = " + to_wstring(fi.CreationTime);
        res += L"; LastAccessTime = " + to_wstring(fi.LastAccessTime);
        res += L"; LastWriteTime = " + to_wstring(fi.LastWriteTime);

        return res;
    }

    unique_ptr<FileInfo> DeserializeFileInfo(const wstring& source) {
        auto parts = helpers::Split(source, g_delim, helpers::SplitOptions::INCLUDE_EMPTY);

        char drive_letter = static_cast<char>(parts[0][0]);

        auto fi = make_unique<FileInfo>(drive_letter);

        fi->ID = helpers::ParseNumber<uint>(parts[1]);
        fi->ParentID = helpers::ParseNumber<uint>(parts[2]);

        auto name_len = helpers::ParseNumber<ushort>(parts[3]);
        fi->CopyAndSetName(reinterpret_cast<const char16_t*>(parts[4].c_str()), name_len);
        fi->FileAttributes = helpers::ParseNumber<uint>(parts[5]);

        fi->SizeReal = helpers::ParseNumber<int>(parts[6]);
        // fi->SizeAllocated = HelperCommon::ParseNumber<uint64>(parts[7]);

        fi->CreationTime = helpers::ParseNumber<uint>(parts[7]);
        fi->LastAccessTime = helpers::ParseNumber<uint>(parts[8]);
        fi->LastWriteTime = helpers::ParseNumber<uint>(parts[9]);

        return fi;
    }

} // namespace indexer_common